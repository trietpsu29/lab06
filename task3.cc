#define UNICODE
#define _UNICODE

#include <windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <cctype>
#include <array>
#include <random>
#include <iomanip>
#include <iostream>

using namespace std;

// ---------- GUI globals (removed iterations/restarts) ----------
HWND hInput, hOutput, hResult, hLog;
volatile bool isRunning = false;

// Helper conversions and logging
static wstring narrow_to_wstring(const string& s) {
    wstring w; w.reserve(s.size());
    for (unsigned char c : s) w.push_back((wchar_t)c);
    return w;
}
static void AppendLogW(const wstring& wmsg) {
    if (!hLog) return;
    int len = GetWindowTextLengthW(hLog);
    SendMessageW(hLog, EM_SETSEL, (WPARAM)len, (LPARAM)len);
    SendMessageW(hLog, EM_REPLACESEL, FALSE, (LPARAM)wmsg.c_str());
}

// ---------- cipher helpers ----------
int mod26(int x) { x %= 26; if (x < 0) x += 26; return x; }
int gcd_int(int a, int b) { return b == 0 ? a : gcd_int(b, a % b); }

// Kasiski
int estimateKeyLength_Kasiski(const string& text) {
    map<string, vector<int> > positions;
    for (int i = 0; i + 3 < (int)text.size(); ++i) {
        string sub = text.substr(i, 3);
        bool ok = true;
        for (size_t k = 0; k < sub.size(); ++k)
            if (!isalpha((unsigned char)sub[k])) { ok = false; break; }
        if (!ok) continue;
        positions[sub].push_back(i);
    }
    vector<int> distances;
    for (auto& kv : positions) {
        vector<int>& v = kv.second;
        if (v.size() >= 2)
            for (size_t j = 1; j < v.size(); ++j) distances.push_back(v[j] - v[j - 1]);
    }
    if (distances.empty()) return 1;
    int g = distances[0];
    for (size_t i = 1; i < distances.size(); ++i) g = gcd_int(g, distances[i]);
    if (g <= 0) g = 1;
    return g;
}

// Index of coincidence
double indexOfCoincidence(const string& s) {
    array<int, 26> freq{}; int total = 0;
    for (char c : s) {
        unsigned char uc = (unsigned char)c;
        if (isalpha(uc)) { freq[tolower(uc) - 'a']++; total++; }
    }
    if (total <= 1) return 0.0;
    double ic = 0.0;
    for (int i = 0; i < 26; ++i) ic += (double)freq[i] * (freq[i] - 1);
    ic /= (double)(total * (total - 1));
    return ic;
}
int estimateKeyLength_IC(const string& text, int maxLen = 20) {
    double bestIC = 0.0; int bestLen = 1;
    for (int k = 1; k <= maxLen; ++k) {
        double avgIC = 0.0;
        for (int i = 0; i < k; ++i) {
            string subset;
            for (int j = i; j < (int)text.size(); j += k) {
                unsigned char uc = (unsigned char)text[j];
                if (isalpha(uc)) subset.push_back(text[j]);
            }
            avgIC += indexOfCoincidence(subset);
        }
        avgIC /= (double)k;
        if (avgIC > bestIC) { bestIC = avgIC; bestLen = k; }
    }
    return bestLen;
}

// English letter frequency
const double ENG_FREQ[26] = {
    0.082,0.015,0.028,0.043,0.127,0.022,0.020,0.061,0.070,0.002,0.008,0.040,0.024,
    0.067,0.075,0.019,0.001,0.060,0.063,0.091,0.028,0.010,0.023,0.001,0.020,0.001
};

int bestShiftForSubset(const string& subset) {
    double bestScore = 1e18; int bestShift = 0;
    for (int shift = 0; shift < 26; ++shift) {
        array<int, 26> freq{}; int total = 0;
        for (char c : subset) {
            unsigned char uc = (unsigned char)c;
            if (isalpha(uc)) {
                int idx = tolower(uc) - 'a';
                int dec = mod26(idx - shift);
                freq[dec]++; total++;
            }
        }
        if (total == 0) continue;
        double chi2 = 0.0;
        for (int i = 0; i < 26; ++i) {
            double expected = ENG_FREQ[i] * (double)total;
            double diff = (double)freq[i] - expected;
            chi2 += diff * diff / (expected + 1e-9);
        }
        if (chi2 < bestScore) { bestScore = chi2; bestShift = shift; }
    }
    return bestShift;
}

string decryptVigenere(const string& text, const string& key) {
    string result; int keyLen = (int)key.size(), pos = 0;
    for (char c : text) {
        unsigned char uc = (unsigned char)c;
        if (isalpha(uc)) {
            int k = tolower(key[pos % keyLen]) - 'a';
            char base = isupper(uc) ? 'A' : 'a';
            char dec = (char)(base + mod26((int)uc - (int)base - k));
            result.push_back(dec); pos++;
        }
        else result.push_back(c);
    }
    return result;
}

// File IO
string read_all_file(const string& path) {
    ifstream fin(path.c_str(), ios::binary);
    if (!fin) return {};
    ostringstream ss; ss << fin.rdbuf(); return ss.str();
}
bool write_all_file(const string& path, const string& content) {
    ofstream fout(path.c_str(), ios::binary);
    if (!fout) return false;
    fout << content; return true;
}

// Worker thread (removed iterations/restarts)
DWORD WINAPI WorkerThread(LPVOID) {
    isRunning = true;
    AppendLogW(L"--- Start decryption ---\r\n");

    char inbuf[1024] = { 0 }, outbuf[1024] = { 0 };
    GetWindowTextA(hInput, inbuf, 1024);
    GetWindowTextA(hOutput, outbuf, 1024);
    string inpath(inbuf), outpath(outbuf);
    if (inpath.empty() || outpath.empty()) {
        AppendLogW(L"Error: input/output path empty\r\n");
        MessageBoxW(NULL, L"Input or output path is empty", L"Error", MB_ICONERROR);
        isRunning = false;
        return 0;
    }

    string cipher = read_all_file(inpath);
    if (cipher.empty()) {
        AppendLogW(L"Error: cannot read input file or file is empty\r\n");
        MessageBoxW(NULL, L"Cannot read input file or file is empty", L"Error", MB_ICONERROR);
        isRunning = false;
        return 0;
    }

    string cleanText;
    cleanText.reserve(cipher.size());
    for (char c : cipher) {
        unsigned char uc = (unsigned char)c;
        if (isalpha(uc)) cleanText.push_back(c);
    }

    int kasiskiLen = estimateKeyLength_Kasiski(cleanText);
    int icLen = estimateKeyLength_IC(cleanText, 20);
    int keyLen = (abs(kasiskiLen - icLen) <= 2) ? kasiskiLen : icLen;

    {
        wstringstream ws;
        ws << L"Estimated key lengths: Kasiski=" << kasiskiLen
            << L", IC=" << icLen << L" => chosen=" << keyLen << L"\r\n";
        AppendLogW(ws.str());
    }

    string key; key.reserve(keyLen);
    for (int i = 0; i < keyLen; ++i) {
        string subset;
        for (int j = i; j < (int)cleanText.size(); j += keyLen)
            subset.push_back(cleanText[j]);
        int shift = bestShiftForSubset(subset);
        key.push_back((char)('a' + shift));
    }

    {
        wstringstream ws;
        ws << L"Estimated key (chi-square): " << narrow_to_wstring(key) << L"\r\n";
        AppendLogW(ws.str());
    }

    string plain = decryptVigenere(cipher, key);

    ostringstream outss;
    outss << key << "\n" << plain;
    if (!write_all_file(outpath, outss.str())) {
        AppendLogW(L"Error: cannot write output file\r\n");
        MessageBoxW(NULL, L"Cannot write output file", L"Error", MB_ICONERROR);
        isRunning = false;
        return 0;
    }

    {
        wstringstream ws;
        ws << L"Decryption complete. Key: " << narrow_to_wstring(key) << L"\r\n";
        AppendLogW(ws.str());
    }
    SetWindowTextW(hResult, (L"Key: " + narrow_to_wstring(key)).c_str());
    MessageBoxW(NULL, L"Decryption finished", L"Info", MB_OK);
    isRunning = false;
    return 0;
}

// Window procedure (removed iteration/restart controls)
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        CreateWindowW(L"static", L"Input file:", WS_VISIBLE | WS_CHILD, 20, 18, 90, 20, hWnd, NULL, NULL, NULL);
        hInput = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            120, 16, 420, 22, hWnd, NULL, NULL, NULL);
        SendMessageW(hInput, EM_LIMITTEXT, 1024, 0);

        CreateWindowW(L"static", L"Output file:", WS_VISIBLE | WS_CHILD, 20, 48, 90, 20, hWnd, NULL, NULL, NULL);
        hOutput = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            120, 46, 420, 22, hWnd, NULL, NULL, NULL);
        SendMessageW(hOutput, EM_LIMITTEXT, 1024, 0);

        CreateWindowW(L"button", L"Decrypt", WS_VISIBLE | WS_CHILD, 20, 80, 120, 28,
            hWnd, (HMENU)1, NULL, NULL);

        hResult = CreateWindowW(L"static", L"", WS_VISIBLE | WS_CHILD, 160, 82, 400, 22,
            hWnd, NULL, NULL, NULL);

        hLog = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER |
            ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
            20, 120, 560, 330, hWnd, NULL, NULL, NULL);
        break;

    case WM_COMMAND:
        if (LOWORD(wp) == 1) {
            if (!isRunning) {
                SetWindowTextW(hLog, L"");
                HANDLE h = CreateThread(NULL, 0, WorkerThread, NULL, 0, NULL);
                if (h) CloseHandle(h);
            }
            else {
                MessageBoxW(hWnd, L"Decryption already running", L"Info", MB_OK);
            }
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProcW(hWnd, msg, wp, lp);
}

// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const wchar_t CLASSNAME[] = L"task3";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASSNAME;
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(0, CLASSNAME, L"task3",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 620, 520,
        NULL, NULL, hInstance, NULL);
    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}