#include <bits/stdc++.h>
#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#endif

using namespace std;
using u8 = uint8_t;
using u32 = uint32_t;
using u64 = uint64_t;
using block8 = array<u8, 8>;

static const int IP[64] = {
  58,50,42,34,26,18,10,2, 60,52,44,36,28,20,12,4,
  62,54,46,38,30,22,14,6,  64,56,48,40,32,24,16,8,
  57,49,41,33,25,17,9,1,   59,51,43,35,27,19,11,3,
  61,53,45,37,29,21,13,5,  63,55,47,39,31,23,15,7
};
static const int FP[64] = {
  40,8,48,16,56,24,64,32, 39,7,47,15,55,23,63,31,
  38,6,46,14,54,22,62,30, 37,5,45,13,53,21,61,29,
  36,4,44,12,52,20,60,28, 35,3,43,11,51,19,59,27,
  34,2,42,10,50,18,58,26, 33,1,41,9,49,17,57,25
};
static const int E[48] = {
  32,1,2,3,4,5, 4,5,6,7,8,9, 8,9,10,11,12,13,
  12,13,14,15,16,17, 16,17,18,19,20,21, 20,21,22,23,24,25,
  24,25,26,27,28,29, 28,29,30,31,32,1
};
static const int P[32] = {
  16,7,20,21,29,12,28,17, 1,15,23,26,5,18,31,10,
  2,8,24,14,32,27,3,9, 19,13,30,6,22,11,4,25
};
static const int PC1[56] = {
  57,49,41,33,25,17,9, 1,58,50,42,34,26,18,
  10,2,59,51,43,35,27,19,11,3,60,52,44,36,
  63,55,47,39,31,23,15,7,62,54,46,38,30,22,
  14,6,61,53,45,37,29,21,13,5,28,20,12,4
};
static const int PC2[48] = {
  14,17,11,24,1,5, 3,28,15,6,21,10, 23,19,12,4,
  26,8,16,7,27,20, 13,2,41,52,31,37, 47,55,30,40,51,45,33,48,44,49,39,56,34,53,46,42,50,36,29,32
};
static const int SHIFTS[16] = {
  1,1,2,2,2,2,2,2, 1,2,2,2,2,2,2,1
};
static const int SBOX[8][4][16] = {
  {{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
   {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
   {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
   {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}},
  {{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
   {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
   {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
   {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}},
  {{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
   {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
   {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
   {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}},
  {{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
   {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
   {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
   {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}},
  {{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
   {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
   {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
   {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}},
  {{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
   {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
   {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
   {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}},
  {{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
   {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
   {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
   {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}},
  {{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
   {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
   {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
   {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}}
};

static inline u64 permute64(u64 in, const int* table, int outBits) {
    u64 out = 0;
    for (int i = 0; i < outBits; ++i) {
        int bitpos = table[i] - 1;
        u64 bit = (in >> (64 - 1 - bitpos)) & 1ULL;
        out = (out << 1) | bit;
    }
    return out;
}
static inline u64 permute48(u64 in, const int* table) {
    u64 out = 0;
    for (int i = 0; i < 48; ++i) {
        int bitpos = table[i] - 1;
        u64 bit = (in >> (64 - 1 - bitpos)) & 1ULL;
        out = (out << 1) | bit;
    }
    return out;
}
static inline u32 permute32(u32 in, const int* table) {
    u32 out = 0;
    for (int i = 0; i < 32; ++i) {
        int bitpos = table[i] - 1;
        u32 bit = (in >> (32 - 1 - bitpos)) & 1U;
        out = (out << 1) | bit;
    }
    return out;
}

u64 bytes_to_u64_be(const block8& b) {
    u64 v = 0;
    for (int i = 0; i < 8; ++i) v = (v << 8) | (u64)b[i];
    return v;
}
block8 u64_to_bytes_be(u64 v) {
    block8 b;
    for (int i = 7; i >= 0; --i) {
        b[i] = (u8)(v & 0xFF);
        v >>= 8;
    }
    return b;
}

u64 expand32to48(u32 r) {
    u64 out = 0;
    for (int i = 0; i < 48; ++i) {
        int bitpos = E[i] - 1;
        u64 bit = ((u64)r >> (32 - 1 - bitpos)) & 1ULL;
        out = (out << 1) | bit;
    }
    return out;
}

u32 sbox_substitute(u64 in48) {
    u32 out32 = 0;
    for (int i = 0; i < 8; ++i) {
        int shift = (7 - i) * 6;
        u8 six = (in48 >> shift) & 0x3F;
        int row = ((six & 0x20) >> 4) | (six & 0x1);
        int col = (six >> 1) & 0xF;
        int s = SBOX[i][row][col];
        out32 = (out32 << 4) | (u32)(s & 0xF);
    }
    return permute32(out32, P);
}

array<u64, 16> buildSubkeys(const block8& keyBytes) {
    u64 key64 = bytes_to_u64_be(keyBytes);
    u64 perm56 = 0;
    for (int i = 0; i < 56; ++i) {
        int pos = PC1[i] - 1;
        u64 bit = (key64 >> (64 - 1 - pos)) & 1ULL;
        perm56 = (perm56 << 1) | bit;
    }
    u32 C = (u32)((perm56 >> 28) & 0x0FFFFFFF);
    u32 D = (u32)(perm56 & 0x0FFFFFFF);

    array<u64, 16> subkeys;
    for (int round = 0; round < 16; ++round) {
        int sh = SHIFTS[round];
        C = ((C << sh) | (C >> (28 - sh))) & 0x0FFFFFFF;
        D = ((D << sh) | (D >> (28 - sh))) & 0x0FFFFFFF;
        u64 CD = ((u64)C << 28) | (u64)D;
        u64 subk = 0;
        for (int i = 0; i < 48; ++i) {
            int pos = PC2[i] - 1;
            u64 bit = (CD >> (56 - 1 - pos)) & 1ULL;
            subk = (subk << 1) | bit;
        }
        subkeys[round] = subk;
    }
    return subkeys;
}

block8 des_block_encrypt(const block8& in, const array<u64, 16>& subkeys) {
    u64 block = bytes_to_u64_be(in);
    u64 perm = 0;
    for (int i = 0; i < 64; ++i) {
        int pos = IP[i] - 1;
        u64 bit = (block >> (64 - 1 - pos)) & 1ULL;
        perm = (perm << 1) | bit;
    }
    u32 L = (u32)((perm >> 32) & 0xFFFFFFFF);
    u32 R = (u32)(perm & 0xFFFFFFFF);
    for (int r = 0; r < 16; ++r) {
        u64 expanded = expand32to48(R);
        u64 x = expanded ^ subkeys[r];
        u32 f = sbox_substitute(x);
        u32 newR = L ^ f;
        L = R;
        R = newR;
    }
    u64 pre = ((u64)R << 32) | (u64)L;
    u64 out = 0;
    for (int i = 0; i < 64; ++i) {
        int pos = FP[i] - 1;
        u64 bit = (pre >> (64 - 1 - pos)) & 1ULL;
        out = (out << 1) | bit;
    }
    return u64_to_bytes_be(out);
}

block8 des_block_decrypt(const block8& in, const array<u64, 16>& subkeys) {
    u64 block = bytes_to_u64_be(in);
    u64 perm = 0;
    for (int i = 0; i < 64; ++i) {
        int pos = IP[i] - 1;
        u64 bit = (block >> (64 - 1 - pos)) & 1ULL;
        perm = (perm << 1) | bit;
    }
    u32 L = (u32)((perm >> 32) & 0xFFFFFFFF);
    u32 R = (u32)(perm & 0xFFFFFFFF);
    for (int r = 0; r < 16; ++r) {
        u64 expanded = expand32to48(R);
        u64 x = expanded ^ subkeys[15 - r];
        u32 f = sbox_substitute(x);
        u32 newR = L ^ f;
        L = R; R = newR;
    }
    u64 pre = ((u64)R << 32) | (u64)L;
    u64 out = 0;
    for (int i = 0; i < 64; ++i) {
        int pos = FP[i] - 1;
        u64 bit = (pre >> (64 - 1 - pos)) & 1ULL;
        out = (out << 1) | bit;
    }
    return u64_to_bytes_be(out);
}

vector<u8> pkcs7_pad(const vector<u8>& data) {
    size_t bs = 8;
    size_t pad = bs - (data.size() % bs);
    if (pad == 0) pad = bs;
    vector<u8> out = data;
    out.insert(out.end(), pad, (u8)pad);
    return out;
}
vector<u8> pkcs7_unpad(const vector<u8>& data) {
    if (data.empty() || (data.size() % 8) != 0) return {};
    u8 last = data.back();
    size_t pad = (size_t)last;
    if (pad == 0 || pad > 8) return {};
    for (size_t i = data.size() - pad; i < data.size(); ++i)
        if (data[i] != last) return {};
    vector<u8> out(data.begin(), data.end() - pad);
    return out;
}

vector<u8> des_ecb_encrypt(const vector<u8>& plaintext, const block8& keyBytes) {
    auto subkeys = buildSubkeys(keyBytes);
    vector<u8> padded = pkcs7_pad(plaintext);
    vector<u8> out;
    out.reserve(padded.size());
    for (size_t i = 0; i < padded.size(); i += 8) {
        block8 b;
        for (int j = 0; j < 8; ++j) b[j] = padded[i + j];
        block8 c = des_block_encrypt(b, subkeys);
        for (int j = 0; j < 8; ++j) out.push_back(c[j]);
    }
    return out;
}
vector<u8> des_ecb_decrypt(const vector<u8>& ciphertext, const block8& keyBytes) {
    if (ciphertext.size() % 8 != 0) return {};
    auto subkeys = buildSubkeys(keyBytes);
    vector<u8> out;
    out.reserve(ciphertext.size());
    for (size_t i = 0; i < ciphertext.size(); i += 8) {
        block8 b;
        for (int j = 0; j < 8; ++j) b[j] = ciphertext[i + j];
        block8 p = des_block_decrypt(b, subkeys);
        for (int j = 0; j < 8; ++j) out.push_back(p[j]);
    }
    return pkcs7_unpad(out);
}

vector<u8> des_cbc_encrypt(const vector<u8>& plaintext, const block8& keyBytes, block8 iv) {
    auto subkeys = buildSubkeys(keyBytes);
    vector<u8> padded = pkcs7_pad(plaintext);
    vector<u8> out;
    out.reserve(padded.size());
    block8 prev = iv;
    for (size_t i = 0; i < padded.size(); i += 8) {
        block8 b;
        for (int j = 0; j < 8; ++j) b[j] = padded[i + j] ^ prev[j];
        block8 c = des_block_encrypt(b, subkeys);
        for (int j = 0; j < 8; ++j) out.push_back(c[j]);
        prev = c;
    }
    return out;
}
vector<u8> des_cbc_decrypt(const vector<u8>& ciphertext, const block8& keyBytes, const block8& iv) {
    if (ciphertext.size() % 8 != 0) return {};
    auto subkeys = buildSubkeys(keyBytes);
    vector<u8> out;
    out.reserve(ciphertext.size());
    block8 prev = iv;
    for (size_t i = 0; i < ciphertext.size(); i += 8) {
        block8 c;
        for (int j = 0; j < 8; ++j) c[j] = ciphertext[i + j];
        block8 p = des_block_decrypt(c, subkeys);
        for (int j = 0; j < 8; ++j) out.push_back(p[j] ^ prev[j]);
        prev = c;
    }
    return pkcs7_unpad(out);
}

block8 random_iv() {
    block8 iv;
    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::uniform_int_distribution<int> dist(0, 255);
    for (int i = 0; i < 8; ++i) iv[i] = (u8)dist(rng);
    return iv;
}

string to_hex(const vector<u8>& v) {
    static const char* hex = "0123456789abcdef";
    string s; s.reserve(v.size() * 2);
    for (u8 b : v) {
        s.push_back(hex[b >> 4]);
        s.push_back(hex[b & 0xF]);
    }
    return s;
}
vector<u8> from_hex(const string& hexstr) {
    string s = hexstr;
    s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());
    if (s.size() % 2 != 0) return {};
    vector<u8> out; out.reserve(s.size() / 2);
    for (size_t i = 0; i < s.size(); i += 2) {
        auto hv = [&](char c)->int {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return 10 + c - 'a';
            if (c >= 'A' && c <= 'F') return 10 + c - 'A';
            return 0;
            };
        int hi = hv(s[i]), lo = hv(s[i + 1]);
        out.push_back((u8)((hi << 4) | lo));
    }
    return out;
}

vector<u8> encrypt_des(const vector<u8>& plaintext, const block8& key, const string& mode, block8* outIV = nullptr) {
    string m = mode;
    for (auto& c : m) c = toupper((unsigned char)c);
    if (m == "ECB") {
        return des_ecb_encrypt(plaintext, key);
    }
    else if (m == "CBC") {
        block8 iv;
        if (outIV) {
            iv = random_iv();
            *outIV = iv;
        }
        else {
            iv = random_iv();
        }
        return des_cbc_encrypt(plaintext, key, iv);
    }
    else {
        throw runtime_error("Mode not supported in this implementation. Supported: ECB, CBC");
    }
}

vector<u8> decrypt_des(const vector<u8>& ciphertext, const block8& key, const string& mode, const block8& iv) {
    string m = mode;
    for (auto& c : m) c = toupper((unsigned char)c);
    if (m == "ECB") {
        return des_ecb_decrypt(ciphertext, key);
    }
    else if (m == "CBC") {
        return des_cbc_decrypt(ciphertext, key, iv);
    }
    else {
        throw runtime_error("Mode not supported in this implementation. Supported: ECB, CBC");
    }
}

#ifdef _WIN32
static HWND gIn, gOut, gKey, gMode, gIV, gLog, gEncryptBtn, gDecryptBtn, gResult, gBrowseIn, gBrowseOut;
static volatile bool gRunning = false;

static wstring narrow_to_wstring(const string& s) {
    wstring w; w.reserve(s.size());
    for (unsigned char c : s) w.push_back((wchar_t)c);
    return w;
}
static void AppendLog(const string& line) {
    if (!gLog) return;
    wstring w = narrow_to_wstring(line + "\r\n");
    int len = GetWindowTextLengthW(gLog);
    SendMessageW(gLog, EM_SETSEL, len, len);
    SendMessageW(gLog, EM_REPLACESEL, FALSE, (LPARAM)w.c_str());
}
static string GetEditA(HWND h) {
    int len = GetWindowTextLengthA(h);
    string s; s.resize(len);
    GetWindowTextA(h, &s[0], len + 1);
    return s;
}
static void SetResult(const string& msg) {
    if (gResult) SetWindowTextW(gResult, narrow_to_wstring(msg).c_str());
}
static bool ReadFileBytes(const string& path, vector<u8>& out) {
    ifstream fin(path.c_str(), ios::binary);
    if (!fin) return false;
    out.assign((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());
    return true;
}
static bool WriteFileBytes(const string& path, const vector<u8>& data) {
    ofstream fout(path.c_str(), ios::binary);
    if (!fout) return false;
    fout.write((const char*)data.data(), data.size());
    return true;
}
static bool WriteFileText(const string& path, const string& data) {
    ofstream fout(path.c_str(), ios::binary);
    if (!fout) return false;
    fout << data;
    return true;
}
static string trim(const string& s) {
    size_t a = 0; while (a < s.size() && isspace((unsigned char)s[a])) a++;
    size_t b = s.size(); while (b > a && isspace((unsigned char)s[b - 1])) b--;
    return s.substr(a, b - a);
}

static string OpenFileDialog(bool save) {
    char buf[MAX_PATH] = { 0 };
    OPENFILENAMEA ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = buf;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "All Files\0*.*\0";
    ofn.Flags = OFN_EXPLORER | (save ? OFN_OVERWRITEPROMPT : OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST);
    BOOL ok = save ? GetSaveFileNameA(&ofn) : GetOpenFileNameA(&ofn);
    if (!ok) return "";
    return string(buf);
}

DWORD WINAPI WorkerThread(LPVOID lp) {
    gRunning = true;
    AppendLog("--- Start ---");

    string inPath = GetEditA(gIn);
    string outPath = GetEditA(gOut);
    string keyHex = GetEditA(gKey);
    string modeSel;
    {
        int idx = (int)SendMessageW(gMode, CB_GETCURSEL, 0, 0);
        wchar_t wbuf[32];
        if (idx >= 0) {
            SendMessageW(gMode, CB_GETLBTEXT, idx, (LPARAM)wbuf);
            wstring ws(wbuf);
            modeSel = string(ws.begin(), ws.end());
        }
    }
    string ivHex = GetEditA(gIV);

    if (inPath.empty() || outPath.empty() || keyHex.size() != 16 || modeSel.empty()) {
        AppendLog("Missing input parameters or key hex incorrect.");
        gRunning = false;
        SetResult("Parameter error");
        return 0;
    }

    auto keyBytesVec = from_hex(keyHex);
    if (keyBytesVec.size() != 8) {
        AppendLog("Invalid key hex.");
        gRunning = false;
        SetResult("Key error");
        return 0;
    }
    block8 key;
    for (int i = 0; i < 8; ++i) key[i] = keyBytesVec[i];

    bool isEncrypt = (lp != NULL);
    vector<u8> fileData;
    if (!ReadFileBytes(inPath, fileData)) {
        AppendLog("Can't read input file.");
        gRunning = false;
        SetResult("Error reading input");
        return 0;
    }

    try {
        if (isEncrypt) {
            vector<u8> cipher;
            block8 outIV;
            string modeU = modeSel; for (auto& c : modeU) c = toupper((unsigned char)c);
            if (modeU == "CBC") {
                cipher = encrypt_des(fileData, key, modeU, &outIV);
                string ivOutHex = to_hex(vector<u8>(outIV.begin(), outIV.end()));
                string cipherHex = to_hex(cipher);
                string all = ivOutHex + "\n" + cipherHex;
                if (!WriteFileText(outPath, all)) {
                    AppendLog("Cannot write output file.");
                    gRunning = false;
                    SetResult("Write output error");
                    return 0;
                }
                AppendLog("CBC encryption done. IV: " + ivOutHex);
                SetResult("Done. IV: " + ivOutHex);
            }
            else {
                cipher = encrypt_des(fileData, key, modeU, nullptr);
                string cipherHex = to_hex(cipher);
                if (!WriteFileText(outPath, cipherHex)) {
                    AppendLog("Cannot write output file.");
                    gRunning = false;
                    SetResult("Write output error");
                    return 0;
                }
                AppendLog("ECB encryption done.");
                SetResult("Done (ECB)");
            }
        }
        else {
            string modeU = modeSel; for (auto& c : modeU) c = toupper((unsigned char)c);
            if (modeU == "ECB") {
                string hexIn((char*)fileData.data(), fileData.size());
                hexIn = trim(hexIn);
                auto cipherBytes = from_hex(hexIn);
                if (cipherBytes.empty()) {
                    AppendLog("Cipher hex error.");
                    gRunning = false;
                    SetResult("Cipher error");
                    return 0;
                }
                auto plain = decrypt_des(cipherBytes, key, modeU, block8{});
                if (plain.empty()) {
                    AppendLog("Decryption failed.");
                    gRunning = false;
                    SetResult("Failed");
                    return 0;
                }
                if (!WriteFileBytes(outPath, plain)) {
                    AppendLog("Cannot write output file.");
                    gRunning = false;
                    SetResult("Write output error");
                    return 0;
                }
                AppendLog("ECB decryption done.");
                SetResult("Done (ECB)");
            }
            else {
                string content((char*)fileData.data(), fileData.size());
                content = trim(content);
                block8 iv{};
                vector<u8> cipherBytes;
                if (!ivHex.empty()) {
                    auto ivv = from_hex(ivHex);
                    if (ivv.size() != 8) {
                        AppendLog("Provided IV hex invalid.");
                        gRunning = false;
                        SetResult("IV error");
                        return 0;
                    }
                    for (int i = 0; i < 8; ++i) iv[i] = ivv[i];
                    auto cb = from_hex(content);
                    cipherBytes = cb;
                }
                else {
                    stringstream ss(content);
                    string ivLine;
                    getline(ss, ivLine);
                    string rest;
                    getline(ss, rest, '\0');
                    auto ivv = from_hex(trim(ivLine));
                    if (ivv.size() != 8) {
                        AppendLog("IV in file is invalid.");
                        gRunning = false;
                        SetResult("IV error");
                        return 0;
                    }
                    for (int i = 0; i < 8; ++i) iv[i] = ivv[i];
                    cipherBytes = from_hex(trim(rest));
                }
                if (cipherBytes.empty()) {
                    AppendLog("Cipher hex error.");
                    gRunning = false;
                    SetResult("Cipher error");
                    return 0;
                }
                auto plain = decrypt_des(cipherBytes, key, modeU, iv);
                if (plain.empty()) {
                    AppendLog("Decryption failed (padding?).");
                    gRunning = false;
                    SetResult("Failed");
                    return 0;
                }
                if (!WriteFileBytes(outPath, plain)) {
                    AppendLog("Cannot write output file.");
                    gRunning = false;
                    SetResult("Write output error");
                    return 0;
                }
                AppendLog("CBC decryption done.");
                SetResult("Done (CBC)");
            }
        }
    }
    catch (exception& e) {
        AppendLog(string("Error: ") + e.what());
        SetResult("Runtime error");
    }

    gRunning = false;
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        CreateWindowW(L"static", L"Input file:", WS_VISIBLE | WS_CHILD, 20, 18, 80, 20, hWnd, NULL, NULL, NULL);
        gIn = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 110, 16, 360, 22, hWnd, NULL, NULL, NULL);
        gBrowseIn = CreateWindowW(L"button", L"Browse", WS_VISIBLE | WS_CHILD, 480, 16, 80, 22, hWnd, (HMENU)101, NULL, NULL);

        CreateWindowW(L"static", L"Output file:", WS_VISIBLE | WS_CHILD, 20, 48, 80, 20, hWnd, NULL, NULL, NULL);
        gOut = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 110, 46, 360, 22, hWnd, NULL, NULL, NULL);
        gBrowseOut = CreateWindowW(L"button", L"Browse", WS_VISIBLE | WS_CHILD, 480, 46, 80, 22, hWnd, (HMENU)102, NULL, NULL);

        CreateWindowW(L"static", L"Key (16 hex):", WS_VISIBLE | WS_CHILD, 20, 78, 100, 20, hWnd, NULL, NULL, NULL);
        gKey = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 130, 76, 180, 22, hWnd, NULL, NULL, NULL);

        CreateWindowW(L"static", L"Mode:", WS_VISIBLE | WS_CHILD, 330, 78, 50, 20, hWnd, NULL, NULL, NULL);
        gMode = CreateWindowW(L"combobox", L"", WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, 380, 76, 100, 200, hWnd, NULL, NULL, NULL);
        SendMessageW(gMode, CB_ADDSTRING, 0, (LPARAM)L"ECB");
        SendMessageW(gMode, CB_ADDSTRING, 0, (LPARAM)L"CBC");
        SendMessageW(gMode, CB_SETCURSEL, 0, 0);

        CreateWindowW(L"static", L"IV (CBC decrypt):", WS_VISIBLE | WS_CHILD, 20, 108, 110, 20, hWnd, NULL, NULL, NULL);
        gIV = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 130, 106, 180, 22, hWnd, NULL, NULL, NULL);

        gEncryptBtn = CreateWindowW(L"button", L"Encrypt", WS_VISIBLE | WS_CHILD, 330, 106, 100, 24, hWnd, (HMENU)1, NULL, NULL);
        gDecryptBtn = CreateWindowW(L"button", L"Decrypt", WS_VISIBLE | WS_CHILD, 440, 106, 100, 24, hWnd, (HMENU)2, NULL, NULL);

        gResult = CreateWindowW(L"static", L"", WS_VISIBLE | WS_CHILD, 20, 136, 560, 20, hWnd, NULL, NULL, NULL);

        gLog = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER |
            ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL,
            20, 160, 560, 300, hWnd, NULL, NULL, NULL);
        break;
    }
    case WM_COMMAND:
        if (LOWORD(wp) == 1) {
            if (!gRunning) {
                SetWindowTextW(gLog, L"");
                HANDLE h = CreateThread(NULL, 0, WorkerThread, (LPVOID)1, 0, NULL);
                if (h) CloseHandle(h);
            }
        }
        else if (LOWORD(wp) == 2) {
            if (!gRunning) {
                SetWindowTextW(gLog, L"");
                HANDLE h = CreateThread(NULL, 0, WorkerThread, NULL, 0, NULL);
                if (h) CloseHandle(h);
            }
        }
        else if (LOWORD(wp) == 101) {
            string p = OpenFileDialog(false);
            if (!p.empty()) SetWindowTextW(gIn, narrow_to_wstring(p).c_str());
        }
        else if (LOWORD(wp) == 102) {
            string p = OpenFileDialog(true);
            if (!p.empty()) SetWindowTextW(gOut, narrow_to_wstring(p).c_str());
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProcW(hWnd, msg, wp, lp);
}

int run_gui() {
    const wchar_t CLASSNAME[] = L"DESGuiClassV1";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = CLASSNAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(0, CLASSNAME, L"task4",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 620, 520,
        NULL, NULL, wc.hInstance, NULL);
    if (!hwnd) return 1;
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}
#endif

int main(int argc, char** argv) {
#ifdef _WIN32
    if (argc == 1 || (argc >= 2 && string(argv[1]) == "gui")) {
        return run_gui();
    }
#endif
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc < 5) {
        cerr << "Usage examples:\n";
        cerr << "  Encrypt: " << argv[0] << " encrypt input.txt key8hex ECB|CBC out_hex.txt\n";
        cerr << "  Decrypt: " << argv[0] << " decrypt in_hex.txt key8hex ECB|CBC out_plain.txt [ivhex-if-CBC]\n";
        cerr << "  GUI: " << argv[0] << " gui\n";
        return 1;
    }
    string cmd = argv[1];
    string infile = argv[2];
    string keyhex = argv[3];
    string mode = argv[4];
    string outfile = (argc >= 6) ? argv[5] : "out.txt";

    if (keyhex.size() != 16) {
        cerr << "Key must be 8 bytes (16 hex chars)\n"; return 1;
    }
    block8 key;
    auto keybytes = from_hex(keyhex);
    if (keybytes.size() != 8) { cerr << "Invalid key hex\n"; return 1; }
    for (int i = 0; i < 8; ++i) key[i] = keybytes[i];

    if (cmd == "encrypt") {
        ifstream fin(infile, ios::binary);
        if (!fin) { cerr << "Cannot open input file\n"; return 1; }
        vector<u8> plain((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());
        fin.close();

        block8 iv = {};
        bool needIV = false;
        for (auto& c : mode) c = toupper((unsigned char)c);
        if (mode == "CBC") needIV = true;

        block8 outIV;
        vector<u8> cipher;
        if (needIV) {
            cipher = encrypt_des(plain, key, mode, &outIV);
        }
        else {
            cipher = encrypt_des(plain, key, mode, nullptr);
        }

        ofstream fout(outfile, ios::binary);
        if (!fout) { cerr << "Cannot open output file\n"; return 1; }
        if (needIV) {
            string ivhex = to_hex(vector<u8>(outIV.begin(), outIV.end()));
            string chex = to_hex(cipher);
            fout << ivhex << "\n" << chex;
            cout << "IV (hex): " << ivhex << "\n";
            cout << "Ciphertext (hex) written to " << outfile << "\n";
        }
        else {
            string chex = to_hex(cipher);
            fout << chex;
            cout << "Ciphertext (hex) written to " << outfile << "\n";
        }
        fout.close();
    }
    else if (cmd == "decrypt") {
        ifstream fin(infile);
        if (!fin) { cerr << "Cannot open input file\n"; return 1; }
        string content; getline(fin, content, '\0'); fin.close();
        block8 iv = {};
        string hexcipher;
        for (auto& c : mode) c = toupper((unsigned char)c);
        if (mode == "CBC") {
            if (argc >= 7) {
                string ivhex = argv[6];
                auto ivv = from_hex(ivhex);
                if (ivv.size() != 8) { cerr << "Invalid IV hex\n"; return 1; }
                for (int i = 0; i < 8; ++i) iv[i] = ivv[i];
                auto trimf = [](string s) { while (!s.empty() && isspace((unsigned char)s.front())) s.erase(s.begin()); while (!s.empty() && isspace((unsigned char)s.back())) s.pop_back(); return s; };
                hexcipher = trimf(content);
            }
            else {
                stringstream ss(content);
                string ivhex; if (!getline(ss, ivhex)) { cerr << "Invalid input (no IV line)\n"; return 1; }
                string rest; getline(ss, rest, '\0');
                auto ivv = from_hex(ivhex);
                if (ivv.size() != 8) { cerr << "Invalid IV hex in file\n"; return 1; }
                for (int i = 0; i < 8; ++i) iv[i] = ivv[i];
                auto trimf = [](string s) { while (!s.empty() && isspace((unsigned char)s.front())) s.erase(s.begin()); while (!s.empty() && isspace((unsigned char)s.back())) s.pop_back(); return s; };
                hexcipher = trimf(rest);
            }
        }
        else {
            auto trimf = [](string s) { while (!s.empty() && isspace((unsigned char)s.front())) s.erase(s.begin()); while (!s.empty() && isspace((unsigned char)s.back())) s.pop_back(); return s; };
            hexcipher = trimf(content);
        }

        auto cipherbytes = from_hex(hexcipher);
        if (cipherbytes.empty()) { cerr << "Invalid ciphertext hex\n"; return 1; }
        vector<u8> plain;
        try {
            plain = decrypt_des(cipherbytes, key, mode, iv);
        }
        catch (exception& e) { cerr << "Error: " << e.what() << "\n"; return 1; }
        if (plain.empty()) { cerr << "Decryption failed (padding or format)\n"; return 1; }

        ofstream fout(outfile, ios::binary);
        if (!fout) { cerr << "Cannot open output file\n"; return 1; }
        fout.write((char*)plain.data(), plain.size());
        fout.close();
        cout << "Plaintext written to " << outfile << "\n";
    }
    else {
        cerr << "Unknown command: use encrypt or decrypt\n";
    }
    return 0;
}