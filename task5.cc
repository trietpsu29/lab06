#include <bits/stdc++.h>
#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#endif

using namespace std;
using u8 = uint8_t;

const int Nb = 4;
const int Nk = 4;
const int Nr = 10;

static const u8 sbox[256] = {
  0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
  0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
  0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
  0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
  0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
  0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
  0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
  0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
  0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
  0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
  0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
  0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
  0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
  0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
  0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
  0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};
static const u8 inv_sbox[256] = {
  0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
  0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
  0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
  0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
  0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
  0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
  0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
  0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
  0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
  0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
  0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
  0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
  0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
  0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
  0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
  0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
};

static const u8 Rcon[11] = { 0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1B,0x36 };

using Block = array<u8, 16>;
using KeySchedule = array<u8, 176>;

static inline u8 xtime(u8 x) { return (u8)((x << 1) ^ ((x >> 7) & 1 ? 0x1b : 0)); }
static inline u8 mul(u8 a, u8 b) {
    u8 p = 0;
    for (int i = 0;i < 8;++i) { if (b & 1)p ^= a; bool hi = a & 0x80; a <<= 1; if (hi)a ^= 0x1b; b >>= 1; }
    return p;
}

KeySchedule expandKey(const vector<u8>& key) {
    KeySchedule w{};
    memcpy(w.data(), key.data(), 16);
    int bytes = 16, rconIter = 1;
    array<u8, 4> temp{};
    while (bytes < 176) {
        for (int i = 0;i < 4;++i) temp[i] = w[bytes - 4 + i];
        if (bytes % 16 == 0) {
            u8 t = temp[0];
            temp[0] = temp[1]; temp[1] = temp[2]; temp[2] = temp[3]; temp[3] = t;
            for (int i = 0;i < 4;++i) temp[i] = sbox[temp[i]];
            temp[0] ^= Rcon[rconIter++];
        }
        for (int i = 0;i < 4;++i) {
            w[bytes] = w[bytes - 16] ^ temp[i];
            bytes++;
        }
    }
    return w;
}

void SubBytes(Block& s) { for (u8& x : s) x = sbox[x]; }
void InvSubBytes(Block& s) { for (u8& x : s) x = inv_sbox[x]; }

void ShiftRows(Block& s) {
    Block t = s;
    s[1] = t[5]; s[5] = t[9]; s[9] = t[13]; s[13] = t[1];
    s[2] = t[10]; s[6] = t[14]; s[10] = t[2]; s[14] = t[6];
    s[3] = t[15]; s[7] = t[3]; s[11] = t[7]; s[15] = t[11];
}
void InvShiftRows(Block& s) {
    Block t = s;
    s[1] = t[13]; s[5] = t[1]; s[9] = t[5]; s[13] = t[9];
    s[2] = t[10]; s[6] = t[14]; s[10] = t[2]; s[14] = t[6];
    s[3] = t[7]; s[7] = t[11]; s[11] = t[15]; s[15] = t[3];
}

void MixColumns(Block& s) {
    for (int c = 0;c < 4;++c) {
        int i = c * 4;
        u8 a0 = s[i], a1 = s[i + 1], a2 = s[i + 2], a3 = s[i + 3];
        s[i] = mul(2, a0) ^ mul(3, a1) ^ a2 ^ a3;
        s[i + 1] = a0 ^ mul(2, a1) ^ mul(3, a2) ^ a3;
        s[i + 2] = a0 ^ a1 ^ mul(2, a2) ^ mul(3, a3);
        s[i + 3] = mul(3, a0) ^ a1 ^ a2 ^ mul(2, a3);
    }
}
void InvMixColumns(Block& s) {
    for (int c = 0;c < 4;++c) {
        int i = c * 4;
        u8 a0 = s[i], a1 = s[i + 1], a2 = s[i + 2], a3 = s[i + 3];
        s[i] = mul(0x0e, a0) ^ mul(0x0b, a1) ^ mul(0x0d, a2) ^ mul(0x09, a3);
        s[i + 1] = mul(0x09, a0) ^ mul(0x0e, a1) ^ mul(0x0b, a2) ^ mul(0x0d, a3);
        s[i + 2] = mul(0x0d, a0) ^ mul(0x09, a1) ^ mul(0x0e, a2) ^ mul(0x0b, a3);
        s[i + 3] = mul(0x0b, a0) ^ mul(0x0d, a1) ^ mul(0x09, a2) ^ mul(0x0e, a3);
    }
}

void AddRoundKey(Block& s, const u8* rk) { for (int i = 0;i < 16;++i) s[i] ^= rk[i]; }

Block aes_encrypt_block(const Block& in, const KeySchedule& w) {
    Block s = in;
    AddRoundKey(s, w.data());
    for (int r = 1;r < Nr;++r) {
        SubBytes(s); ShiftRows(s); MixColumns(s);
        AddRoundKey(s, w.data() + r * 16);
    }
    SubBytes(s); ShiftRows(s);
    AddRoundKey(s, w.data() + Nr * 16);
    return s;
}
Block aes_decrypt_block(const Block& in, const KeySchedule& w) {
    Block s = in;
    AddRoundKey(s, w.data() + Nr * 16);
    for (int r = Nr - 1;r > 0;--r) {
        InvShiftRows(s); InvSubBytes(s);
        AddRoundKey(s, w.data() + r * 16);
        InvMixColumns(s);
    }
    InvShiftRows(s); InvSubBytes(s);
    AddRoundKey(s, w.data());
    return s;
}

vector<u8> pkcs7_pad(const vector<u8>& v) {
    size_t bs = 16, p = bs - (v.size() % bs); if (p == 0)p = bs;
    vector<u8>out = v; out.insert(out.end(), p, (u8)p); return out;
}
vector<u8> pkcs7_unpad(const vector<u8>& v) {
    if (v.empty() || v.size() % 16) return {};
    u8 p = v.back(); if (p == 0 || p > 16) return {};
    for (size_t i = v.size() - p;i < v.size();++i) if (v[i] != p) return {};
    return vector<u8>(v.begin(), v.end() - p);
}

Block random_iv() {
    Block iv; random_device rd; for (auto& i : iv)i = (u8)(rd() & 0xFF); return iv;
}
string to_hex(const vector<u8>& v) {
    static const char* h = "0123456789abcdef"; string s; s.reserve(v.size() * 2);
    for (u8 b : v) { s.push_back(h[b >> 4]); s.push_back(h[b & 0xF]); } return s;
}
vector<u8> from_hex(const string& s_) {
    string t; t.reserve(s_.size());
    for (char c : s_) if (!isspace((unsigned char)c)) t += c;
    if (t.size() % 2) return {}; vector<u8>out;
    auto hv = [](char c) {if ('0' <= c && c <= '9')return c - '0'; if ('a' <= c && c <= 'f')return 10 + c - 'a'; if ('A' <= c && c <= 'F')return 10 + c - 'A'; return 0;};
    for (size_t i = 0;i < t.size();i += 2) out.push_back((u8)((hv(t[i]) << 4) | hv(t[i + 1])));
    return out;
}

vector<u8> aes_ecb_encrypt(const vector<u8>& plain, const vector<u8>& key) {
    KeySchedule ks = expandKey(key);
    auto data = pkcs7_pad(plain);
    vector<u8>out; out.reserve(data.size());
    for (size_t i = 0;i < data.size();i += 16) {
        Block blk; memcpy(blk.data(), &data[i], 16);
        Block enc = aes_encrypt_block(blk, ks);
        out.insert(out.end(), enc.begin(), enc.end());
    }
    return out;
}
vector<u8> aes_ecb_decrypt(const vector<u8>& cipher, const vector<u8>& key) {
    if (cipher.size() % 16) return {};
    KeySchedule ks = expandKey(key);
    vector<u8>out;
    for (size_t i = 0;i < cipher.size();i += 16) {
        Block blk; memcpy(blk.data(), &cipher[i], 16);
        Block dec = aes_decrypt_block(blk, ks);
        out.insert(out.end(), dec.begin(), dec.end());
    }
    return pkcs7_unpad(out);
}
vector<u8> aes_cbc_encrypt(const vector<u8>& plain, const vector<u8>& key, Block iv) {
    KeySchedule ks = expandKey(key);
    auto data = pkcs7_pad(plain);
    vector<u8>out; out.reserve(data.size());
    Block prev = iv;
    for (size_t i = 0;i < data.size();i += 16) {
        Block blk; memcpy(blk.data(), &data[i], 16);
        for (int j = 0;j < 16;++j) blk[j] ^= prev[j];
        Block enc = aes_encrypt_block(blk, ks);
        out.insert(out.end(), enc.begin(), enc.end());
        prev = enc;
    }
    return out;
}
vector<u8> aes_cbc_decrypt(const vector<u8>& cipher, const vector<u8>& key, const Block& iv) {
    if (cipher.size() % 16) return {};
    KeySchedule ks = expandKey(key);
    vector<u8>out;
    Block prev = iv;
    for (size_t i = 0;i < cipher.size();i += 16) {
        Block blk; memcpy(blk.data(), &cipher[i], 16);
        Block dec = aes_decrypt_block(blk, ks);
        for (int j = 0;j < 16;++j) dec[j] ^= prev[j];
        out.insert(out.end(), dec.begin(), dec.end());
        prev = blk;
    }
    return pkcs7_unpad(out);
}

vector<u8> encrypt_aes(const vector<u8>& plain, const vector<u8>& key, const string& mode, Block* outIV = nullptr) {
    string m = mode; for (auto& c : m)c = toupper((unsigned char)c);
    if (m == "ECB") return aes_ecb_encrypt(plain, key);
    else if (m == "CBC") { Block iv = random_iv(); if (outIV)*outIV = iv; return aes_cbc_encrypt(plain, key, iv); }
    throw runtime_error("Unsupported mode");
}
vector<u8> decrypt_aes(const vector<u8>& cipher, const vector<u8>& key, const string& mode, const Block& iv) {
    string m = mode; for (auto& c : m)c = toupper((unsigned char)c);
    if (m == "ECB") return aes_ecb_decrypt(cipher, key);
    else if (m == "CBC") return aes_cbc_decrypt(cipher, key, iv);
    throw runtime_error("Unsupported mode");
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
    ofn.lpstrFile = buf;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "All Files\0*.*\0";
    ofn.Flags = OFN_EXPLORER | (save ? OFN_OVERWRITEPROMPT : (OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST));
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

    if (inPath.empty() || outPath.empty() || keyHex.size() != 32 || modeSel.empty()) {
        AppendLog("Missing parameters or invalid key length (must be 32 hex chars).");
        SetResult("Parameter error");
        gRunning = false;
        return 0;
    }
    vector<u8> key = from_hex(keyHex);
    if (key.size() != 16) {
        AppendLog("Invalid key hex.");
        SetResult("Key error");
        gRunning = false;
        return 0;
    }
    bool isEncrypt = (lp != NULL);
    vector<u8> fileData;
    if (!ReadFileBytes(inPath, fileData)) {
        AppendLog("Cannot read input file.");
        SetResult("Read error");
        gRunning = false;
        return 0;
    }
    try {
        if (isEncrypt) {
            vector<u8> cipher;
            Block outIV{};
            string modeU = modeSel; for (auto& c : modeU) c = toupper((unsigned char)c);
            if (modeU == "CBC") {
                cipher = encrypt_aes(fileData, key, modeU, &outIV);
                string ivHexOut = to_hex(vector<u8>(outIV.begin(), outIV.end()));
                string cipherHex = to_hex(cipher);
                string all = ivHexOut + "\n" + cipherHex; // separated IV line
                if (!WriteFileText(outPath, all)) {
                    AppendLog("Cannot write output file.");
                    SetResult("Write error");
                    gRunning = false;
                    return 0;
                }
                AppendLog("CBC encryption done. IV: " + ivHexOut);
                SetResult("Done CBC");
            }
            else {
                cipher = encrypt_aes(fileData, key, modeU, nullptr);
                string cipherHex = to_hex(cipher);
                if (!WriteFileText(outPath, cipherHex)) {
                    AppendLog("Cannot write output file.");
                    SetResult("Write error");
                    gRunning = false;
                    return 0;
                }
                AppendLog("ECB encryption done.");
                SetResult("Done ECB");
            }
        }
        else {
            string modeU = modeSel; for (auto& c : modeU) c = toupper((unsigned char)c);
            string content((char*)fileData.data(), fileData.size());
            content.erase(remove(content.begin(), content.end(), '\n'), content.end());
            content.erase(remove(content.begin(), content.end(), '\r'), content.end());
            Block iv{};
            vector<u8> cipher;
            if (modeU == "CBC") {
                if (!ivHex.empty()) {
                    vector<u8> ivv = from_hex(ivHex);
                    if (ivv.size() != 16) {
                        AppendLog("Provided IV invalid.");
                        SetResult("IV error");
                        gRunning = false;
                        return 0;
                    }
                    memcpy(iv.data(), ivv.data(), 16);
                    cipher = from_hex(content);
                }
                else {
                    if (content.size() < 32) {
                        AppendLog("Ciphertext too short for CBC (missing IV).");
                        SetResult("Too short");
                        gRunning = false;
                        return 0;
                    }
                    vector<u8> ivv = from_hex(content.substr(0, 32));
                    if (ivv.size() != 16) {
                        AppendLog("IV parsing failed.");
                        SetResult("IV error");
                        gRunning = false;
                        return 0;
                    }
                    memcpy(iv.data(), ivv.data(), 16);
                    cipher = from_hex(content.substr(32));
                }
            }
            else {
                cipher = from_hex(content);
            }
            if (cipher.empty()) {
                AppendLog("Cipher hex invalid.");
                SetResult("Cipher error");
                gRunning = false;
                return 0;
            }
            vector<u8> plain = decrypt_aes(cipher, key, modeU, iv);
            if (plain.empty()) {
                AppendLog("Decryption failed (padding or format).");
                SetResult("Failed");
                gRunning = false;
                return 0;
            }
            if (!WriteFileBytes(outPath, plain)) {
                AppendLog("Cannot write output file.");
                SetResult("Write error");
                gRunning = false;
                return 0;
            }
            AppendLog(string(modeU == "CBC" ? "CBC" : "ECB") + " decryption done.");
            SetResult("Done");
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

        CreateWindowW(L"static", L"Key (32 hex):", WS_VISIBLE | WS_CHILD, 20, 78, 100, 20, hWnd, NULL, NULL, NULL);
        gKey = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 130, 76, 180, 22, hWnd, NULL, NULL, NULL);

        CreateWindowW(L"static", L"Mode:", WS_VISIBLE | WS_CHILD, 330, 78, 50, 20, hWnd, NULL, NULL, NULL);
        gMode = CreateWindowW(L"combobox", L"", WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, 380, 76, 100, 200, hWnd, NULL, NULL, NULL);
        SendMessageW(gMode, CB_ADDSTRING, 0, (LPARAM)L"ECB");
        SendMessageW(gMode, CB_ADDSTRING, 0, (LPARAM)L"CBC");
        SendMessageW(gMode, CB_SETCURSEL, 0, 0);

        CreateWindowW(L"static", L"IV (CBC decrypt):", WS_VISIBLE | WS_CHILD, 20, 108, 120, 20, hWnd, NULL, NULL, NULL);
        gIV = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 150, 106, 160, 22, hWnd, NULL, NULL, NULL);

        gEncryptBtn = CreateWindowW(L"button", L"Encrypt", WS_VISIBLE | WS_CHILD, 330, 106, 100, 24, hWnd, (HMENU)1, NULL, NULL);
        gDecryptBtn = CreateWindowW(L"button", L"Decrypt", WS_VISIBLE | WS_CHILD, 440, 106, 100, 24, hWnd, (HMENU)2, NULL, NULL);

        gResult = CreateWindowW(L"static", L"", WS_VISIBLE | WS_CHILD, 20, 136, 560, 20, hWnd, NULL, NULL, NULL);

        gLog = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL,
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
    const wchar_t CLASSNAME[] = L"AESGuiClassV1";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = CLASSNAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassW(&wc);
    HWND hwnd = CreateWindowExW(0, CLASSNAME, L"task5", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 620, 520,
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
    if (argc == 1 || (argc >= 2 && string(argv[1]) == "gui")) return run_gui();
#endif
    ios::sync_with_stdio(false);
    if (argc < 6) {
        cerr << "Usage:\n"
            << "  Encrypt: " << argv[0] << " encrypt input.txt key32hex ECB|CBC out.txt\n"
            << "  Decrypt: " << argv[0] << " decrypt cipher.txt key32hex ECB|CBC out.txt\n"
            << "  GUI: " << argv[0] << " gui\n";
        return 1;
    }
    string op = argv[1];
    string inputFile = argv[2];
    string key_hex = argv[3];
    string mode = argv[4];
    string outputFile = argv[5];

    vector<u8> key = from_hex(key_hex);
    if (key.size() != 16) {
        cerr << "Key must be 16 bytes (32 hex chars) for AES-128\n";
        return 1;
    }

    if (op == "encrypt") {
        ifstream fin(inputFile, ios::binary);
        if (!fin) { cerr << "Cannot open input file\n"; return 1; }
        vector<u8> plain((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());
        fin.close();
        Block iv{};
        vector<u8> cipher = encrypt_aes(plain, key, mode, &iv);
        ofstream fout(outputFile);
        if (!fout) { cerr << "Cannot open output file\n"; return 1; }
        if (mode == "CBC") {
            fout << to_hex(vector<u8>(iv.begin(), iv.end())) << "\n"; // newline after IV
        }
        fout << to_hex(cipher);
        fout.close();
        cout << "Encryption done.\n";
        if (mode == "CBC") cout << "IV stored as first line (32 hex chars).\n";
    }
    else if (op == "decrypt") {
        ifstream fin(inputFile);
        if (!fin) { cerr << "Cannot open ciphertext file\n"; return 1; }
        string all_hex((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());
        fin.close();
        all_hex.erase(remove(all_hex.begin(), all_hex.end(), '\n'), all_hex.end());
        all_hex.erase(remove(all_hex.begin(), all_hex.end(), '\r'), all_hex.end());
        Block iv{};
        vector<u8> cipher;
        if (mode == "CBC") {
            if (all_hex.size() < 32) { cerr << "Ciphertext too short (missing IV)\n"; return 1; }
            vector<u8> iv_bytes = from_hex(all_hex.substr(0, 32));
            if (iv_bytes.size() != 16) { cerr << "Invalid IV\n"; return 1; }
            memcpy(iv.data(), iv_bytes.data(), 16);
            cipher = from_hex(all_hex.substr(32));
        }
        else {
            cipher = from_hex(all_hex);
        }
        vector<u8> plain = decrypt_aes(cipher, key, mode, iv);
        ofstream fout(outputFile, ios::binary);
        if (!fout) { cerr << "Cannot open output file\n"; return 1; }
        fout.write((char*)plain.data(), plain.size());
        fout.close();
        cout << "Decryption done.\n";
    }
    else {
        cerr << "Invalid operation: encrypt or decrypt\n";
        return 1;
    }
    return 0;
}