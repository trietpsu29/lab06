#define UNICODE
#define _UNICODE

#include <windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <random>
#include <array>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

// ====================================================================================
// GLOBAL UI HANDLES
// ====================================================================================
HWND hInput, hOutput, hIter, hRestarts, hResult, hLog;

// Thread running?
bool isRunning = false;

// ====================================================================================
// Helper: append log text to multiline EDIT
// ====================================================================================
void AppendLog(const wstring& msg)
{
    int len = GetWindowTextLengthW(hLog);
    SendMessageW(hLog, EM_SETSEL, len, len);
    SendMessageW(hLog, EM_REPLACESEL, FALSE, (LPARAM)msg.c_str());
}

// ====================================================================================
// Your cryptanalysis data
// ====================================================================================
static const char* COMMON_WORDS_ARR[] = {
    "the","be","to","of","and","a","in","that","have","i","it","for","not","on","with","he","as",
    "you","do","at","this","but","his","by","from","they","we","say","her","she","or","an","will",
    "my","one","all","would","there","their","what","so","up","out","if","about","who","get","which",
    "go","me","when","make","can","like","time","no","just","him","know","take","people","into","year",
    "your","good","some","could","them","see","other","than","then","now","look","only","come","its",
    "over","think","also","back","after","use","two","how","our","work","first","well","way","even",
    "new","want","because","any","these","give","day","most","us","is","are","was","were","has","had",
    "been","were","wasn't","don't","can't","did","made","during","such","many","through","per","among"
};
static unordered_set<string> COMMON_WORDS;

static const pair<const char*, double> COMMON_BIGRAMS_ARR[] = {
    {"th",2.0}, {"he",1.9}, {"in",1.6}, {"er",1.5}, {"an",1.4}, {"re",1.2}, {"on",1.2},
    {"at",1.1}, {"en",1.1}, {"nd",1.0}, {"ti",1.0}, {"es",0.95}, {"or",0.9}, {"te",0.9},
    {"of",1.3}, {"ed",0.9}, {"is",1.4}, {"it",1.2}, {"al",0.7}, {"ar",0.7}, {"st",0.8}
};
static unordered_map<string, double> BIGRAM_WEIGHT;

// ====================================================================================
// File IO
// ====================================================================================
string read_file_utf8(const string& path) {
    ifstream in(path, ios::binary);
    if (!in) return "";
    ostringstream ss; ss << in.rdbuf();
    return ss.str();
}
bool write_file_utf8(const string& path, const string& content) {
    ofstream out(path, ios::binary);
    if (!out) return false;
    out << content;
    return true;
}

// ====================================================================================
// Helpers
// ====================================================================================
string normalize_lower(const string& s) {
    string o; o.reserve(s.size());
    for (unsigned char c : s) {
        if (c >= 'A' && c <= 'Z') o.push_back(char(c - 'A' + 'a'));
        else o.push_back(c);
    }
    return o;
}

string apply_mapping(const string& norm_text, const array<char, 26>& map_c2p) {
    string out; out.reserve(norm_text.size());
    for (unsigned char uc : norm_text) {
        char c = char(uc);
        if (c >= 'a' && c <= 'z') out.push_back(map_c2p[c - 'a']);
        else out.push_back(c);
    }
    return out;
}

vector<string> extract_words(const string& s) {
    vector<string> words;
    string w;
    for (char ch : s) {
        if (ch >= 'a' && ch <= 'z') w.push_back(ch);
        else {
            if (!w.empty()) { words.push_back(w); w.clear(); }
        }
    }
    if (!w.empty()) words.push_back(w);
    return words;
}

// ====================================================================================
// Scoring
// ====================================================================================
double score_plaintext(const string& plain) {
    vector<string> words = extract_words(plain);
    double word_score = 0.0;
    for (const string& w : words) {
        if (w.size() <= 1) continue;
        if (COMMON_WORDS.find(w) != COMMON_WORDS.end()) {
            word_score += min(3.0, double(w.size()) / 3.0 + 0.5);
        }
    }

    double bigram_score = 0.0;
    for (size_t i = 1; i < plain.size(); ++i) {
        char a = plain[i - 1], b = plain[i];
        if (a >= 'a' && a <= 'z' && b >= 'a' && b <= 'z') {
            string bg; bg.push_back(a); bg.push_back(b);
            auto it = BIGRAM_WEIGHT.find(bg);
            if (it != BIGRAM_WEIGHT.end()) bigram_score += it->second;
        }
    }

    size_t letters = 0;
    for (char c : plain) if (c >= 'a' && c <= 'z') letters++;

    return word_score * 5.0 + bigram_score + (double(letters) / max<size_t>(1, plain.size())) * 2.0;
}

array<char, 26> init_freq_map(const string& text) {
    array<int, 26> freq{}; freq.fill(0);
    for (char c : text) if (c >= 'a' && c <= 'z') freq[c - 'a']++;

    vector<pair<int, int>> v;
    for (int i = 0;i < 26;i++) v.emplace_back(freq[i], i);
    sort(v.begin(), v.end(),
        [](const pair<int, int>& a, const pair<int, int>& b) {
            if (a.first != b.first) return a.first > b.first;
            return a.second < b.second;
        }
    );

    const string order = "etaoinshrdlcumwfgypbvkjxqz";
    array<char, 26> map_c2p;
    for (int i = 0;i < 26;i++) map_c2p[i] = 'a' + i;
    for (int i = 0;i < 26;i++) {
        int idx = v[i].second;
        map_c2p[idx] = order[i];
    }
    return map_c2p;
}

string mapping_to_string(const array<char, 26>& m) {
    string out;
    for (int i = 0;i < 26;i++) {
        out += char('a' + i);
        out += "->";
        out += m[i];
        if (i < 25) out += ' ';
    }
    return out;
}

void swap_mapping(array<char, 26>& m, int i, int j) {
    char t = m[i];
    m[i] = m[j];
    m[j] = t;
}

array<char, 26> optimize_mapping(const string& norm,
    array<char, 26> start,
    int iterations,
    mt19937& rng)
{
    array<char, 26> best = start;
    string best_plain = apply_mapping(norm, best);
    double best_score = score_plaintext(best_plain);

    array<char, 26> cur = best;
    double cur_score = best_score;

    uniform_int_distribution<int> dist26(0, 25);
    uniform_real_distribution<double> U(0.0, 1.0);

    double T = 1.0;
    double cooling = pow(1e-4, 1.0 / max(iterations, 1));

    for (int it = 0; it < iterations; it++)
    {
        int a = dist26(rng), b = dist26(rng);
        if (a == b) b = (a + 1) % 26;

        swap_mapping(cur, a, b);
        double sc = score_plaintext(apply_mapping(norm, cur));

        double delta = sc - cur_score;

        if (delta >= 0 || U(rng) < exp(delta / max(1e-9, T))) {
            cur_score = sc;
            if (sc > best_score) {
                best_score = sc;
                best = cur;
            }
        }
        else {
            swap_mapping(cur, a, b);
        }

        if (it % 5000 == 0) {
            wstringstream ws;
            ws << L"[opt] iter=" << it << L" score=" << cur_score << L"\n";
            AppendLog(ws.str());
        }

        T *= cooling;
        if (T < 1e-8) T = 1e-8;
    }

    return best;
}

// ====================================================================================
// Worker thread
// ====================================================================================
DWORD WINAPI DecryptThread(LPVOID lp)
{
    isRunning = true;
    AppendLog(L"--- Bắt đầu giải mã ---\n");

    char inputPath[1024], outputPath[1024], itBuf[32], rsBuf[32];
    GetWindowTextA(hInput, inputPath, 1024);
    GetWindowTextA(hOutput, outputPath, 1024);
    GetWindowTextA(hIter, itBuf, 32);
    GetWindowTextA(hRestarts, rsBuf, 32);

    int iterations = strlen(itBuf) ? atoi(itBuf) : 20000;
    int restarts = strlen(rsBuf) ? atoi(rsBuf) : 8;
    if (iterations < 1000) iterations = 1000;
    if (restarts < 1) restarts = 1;

    if (COMMON_WORDS.empty()) {
        for (auto w : COMMON_WORDS_ARR) COMMON_WORDS.insert(w);
    }
    if (BIGRAM_WEIGHT.empty()) {
        for (auto& p : COMMON_BIGRAMS_ARR) BIGRAM_WEIGHT[p.first] = p.second;
    }

    string raw = read_file_utf8(inputPath);
    if (raw.empty()) {
        MessageBoxW(NULL, L"Không thể đọc file input!", L"Lỗi", MB_ICONERROR);
        isRunning = false;
        return 0;
    }

    string norm = normalize_lower(raw);
    array<char, 26> base_map = init_freq_map(norm);

    mt19937 rng(random_device{}());

    array<char, 26> global_best = base_map;
    string global_plain = apply_mapping(norm, global_best);
    double global_score = score_plaintext(global_plain);

    for (int r = 0; r < restarts; r++) {
        wstringstream ws;
        ws << L"[restart " << (r + 1) << L"/" << restarts << L"]\n";
        AppendLog(ws.str());

        array<char, 26> start = base_map;

        uniform_int_distribution<int> dist(0, 25);
        for (int k = 0;k < 40 + r * 10;k++) {
            int a = dist(rng), b = dist(rng);
            if (a != b) swap_mapping(start, a, b);
        }

        array<char, 26> best_local = optimize_mapping(norm, start, iterations, rng);
        string plain_local = apply_mapping(norm, best_local);
        double sc_local = score_plaintext(plain_local);

        wstringstream ws2;
        ws2 << L"Local score = " << sc_local << L"\n";
        AppendLog(ws2.str());

        if (sc_local > global_score) {
            global_score = sc_local;
            global_best = best_local;
            global_plain = plain_local;
            AppendLog(L"--> New global best!\n");
        }
    }

    ostringstream out;
    out << fixed << setprecision(6) << global_score << "\n";
    out << mapping_to_string(global_best) << "\n";
    out << global_plain;

    write_file_utf8(outputPath, out.str());

    wstringstream ws;
    ws << L"Hoàn tất! Score = " << global_score << L"\n";
    SetWindowTextW(hResult, ws.str().c_str());
    AppendLog(L"--- Hoàn tất giải mã ---\n");

    MessageBoxW(NULL, L"Giải mã xong!", L"Thông báo", MB_OK);

    isRunning = false;
    return 0;
}

// ====================================================================================
// Window Proc
// ====================================================================================
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
    switch (msg)
    {
    case WM_CREATE:
        CreateWindowW(L"static", L"Input:", WS_CHILD | WS_VISIBLE,
            20, 20, 100, 20, hwnd, NULL, NULL, NULL);
        hInput = CreateWindowW(L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            120, 20, 350, 22, hwnd, NULL, NULL, NULL);

        CreateWindowW(L"static", L"Output:", WS_CHILD | WS_VISIBLE,
            20, 60, 100, 20, hwnd, NULL, NULL, NULL);
        hOutput = CreateWindowW(L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            120, 60, 350, 22, hwnd, NULL, NULL, NULL);

        CreateWindowW(L"static", L"Iterations:", WS_CHILD | WS_VISIBLE,
            20, 100, 100, 20, hwnd, NULL, NULL, NULL);
        hIter = CreateWindowW(L"edit", L"20000", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            120, 100, 100, 22, hwnd, NULL, NULL, NULL);

        CreateWindowW(L"static", L"Restarts:", WS_CHILD | WS_VISIBLE,
            240, 100, 100, 20, hwnd, NULL, NULL, NULL);
        hRestarts = CreateWindowW(L"edit", L"8", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            330, 100, 100, 22, hwnd, NULL, NULL, NULL);

        CreateWindowW(L"button", L"Decrypt", WS_CHILD | WS_VISIBLE,
            20, 140, 120, 30, hwnd, (HMENU)1, NULL, NULL);

        hResult = CreateWindowW(L"static", L"", WS_CHILD | WS_VISIBLE,
            20, 180, 450, 25, hwnd, NULL, NULL, NULL);

        // ---------- MULTILINE LOG BOX ----------
        hLog = CreateWindowW(
            L"edit", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER |
            ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_READONLY,
            20, 220, 520, 200,
            hwnd, NULL, NULL, NULL
        );
        break;

    case WM_COMMAND:
        if (LOWORD(w) == 1 && !isRunning)
        {
            CreateThread(NULL, 0, DecryptThread, NULL, 0, NULL);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProcW(hwnd, msg, w, l);
}

// ====================================================================================
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int n)
{
    const wchar_t CLASS_NAME[] = L"SubstApp";

    WNDCLASSW wc = {};
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS_NAME;
    wc.lpfnWndProc = WndProc;

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowW(CLASS_NAME, L"task2",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        600, 500,
        NULL, NULL, hInst, NULL);

    ShowWindow(hwnd, n);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}
