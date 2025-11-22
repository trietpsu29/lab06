#define UNICODE
#define _UNICODE

#include <windows.h>
#include <commdlg.h>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <climits>
using namespace std;

string decryptCaesar(const string& text, int k) {
    string res = text;
    for (char& c : res) {
        if ('A' <= c && c <= 'Z')
            c = char('A' + (c - 'A' - k + 26) % 26);
        else if ('a' <= c && c <= 'z')
            c = char('a' + (c - 'a' - k + 26) % 26);
    }
    return res;
}

vector<string> extract_words_lower(const string& text) {
    vector<string> words;
    string cur;
    for (unsigned char uc : text) {
        char c = static_cast<char>(uc);
        if (isalpha((unsigned char)c) && (unsigned char)c < 128) {
            cur.push_back(char(tolower((unsigned char)c)));
        }
        else {
            if (!cur.empty()) {
                words.push_back(cur);
                cur.clear();
            }
        }
    }
    if (!cur.empty()) words.push_back(cur);
    return words;
}

long long score_by_common_words(const string& plain) {
    static const unordered_map<string, int> weights = {
        {"the", 12}, {"and", 8}, {"that",6}, {"have",5}, {"for",4},
        {"not",4}, {"with",4}, {"you",5}, {"this",6}, {"but",4},
        {"from",3}, {"they",3}, {"his",3}, {"her",3}, {"was",4},
        {"are",4}, {"which",4}, {"one",3}, {"all",3}, {"there",4},
        {"their",3}, {"what",3}, {"when",3}, {"your",3}, {"can",3},
        {"about",3}, {"been",3}, {"has",3}
    };

    vector<string> words = extract_words_lower(plain);
    if (words.empty()) return LLONG_MIN / 4;

    unordered_map<string, int> freq;
    for (auto& w : words) freq[w]++;

    long long score = 0;
    for (const auto& p : weights) {
        auto it = freq.find(p.first);
        if (it != freq.end()) score += 1LL * it->second * p.second;
    }

    score += (long long)words.size() / 50;
    return score;
}

HWND hInput, hOutput, hResult;

string ReadFile(const string& path) {
    ifstream fin(path, ios::binary);
    if (!fin) return "";
    stringstream ss;
    ss << fin.rdbuf();
    return ss.str();
}

bool WriteFileOut(const string& path, const string& data) {
    ofstream fout(path, ios::binary);
    if (!fout) return false;
    fout << data;
    return true;
}

void DoDecrypt(HWND hwnd) {
    char inputPath[4096], outputPath[4096];
    GetWindowTextA(hInput, inputPath, 4096);
    GetWindowTextA(hOutput, outputPath, 4096);

    if (strlen(inputPath) == 0 || strlen(outputPath) == 0) {
        MessageBoxW(hwnd, L"Vui lòng nhập đầy đủ đường dẫn file.", L"Lỗi", MB_ICONERROR);
        return;
    }

    string cipher = ReadFile(inputPath);
    if (cipher.empty()) {
        MessageBoxW(hwnd, L"Không đọc được file input.", L"Lỗi", MB_ICONERROR);
        return;
    }

    int bestKey = 0;
    long long bestScore = LLONG_MIN;
    string bestPlain;

    for (int k = 0; k < 26; ++k) {
        string cand = decryptCaesar(cipher, k);
        long long sc = score_by_common_words(cand);
        if (sc > bestScore) {
            bestScore = sc;
            bestKey = k;
            bestPlain = cand;
        }
    }

    string out = to_string(bestKey) + "\n" + bestPlain;

    if (!WriteFileOut(outputPath, out)) {
        MessageBoxW(hwnd, L"Không ghi được file output.", L"Lỗi", MB_ICONERROR);
        return;
    }

    wstringstream ws;
    ws << bestKey;
    wstring msg = L"Giải mã thành công! Key: " + ws.str();
    SetWindowTextW(hResult, msg.c_str());

    MessageBoxW(hwnd, L"Hoàn tất!", L"Thông báo", MB_ICONINFORMATION);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        CreateWindowW(L"static", L"Input file:", WS_VISIBLE | WS_CHILD,
            20, 20, 100, 20, hwnd, NULL, NULL, NULL);

        hInput = CreateWindowW(
            L"edit", L"",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            130, 20, 300, 22,
            hwnd, NULL, NULL, NULL
        );

        CreateWindowW(L"static", L"Output file:", WS_VISIBLE | WS_CHILD,
            20, 60, 100, 20, hwnd, NULL, NULL, NULL);

        hOutput = CreateWindowW(
            L"edit", L"",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            130, 60, 300, 22,
            hwnd, NULL, NULL, NULL
        );

        CreateWindowW(L"button", L"Decrypt", WS_VISIBLE | WS_CHILD,
            20, 100, 100, 30, hwnd, (HMENU)1, NULL, NULL);

        hResult = CreateWindowW(
            L"static", L"", WS_VISIBLE | WS_CHILD,
            20, 150, 400, 25, hwnd, NULL, NULL, NULL
        );
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) DoDecrypt(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR pCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"CaesarApp";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"task1",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 480, 240,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}

