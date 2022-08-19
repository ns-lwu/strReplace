#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <string_view>
#include <chrono>

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

static void escape(void *p){
    asm volatile("" : : "g"(p) : "memory");
}
static void clobber(){
    asm volatile("" : : : "memory");
}

class EscapeCharTable {
public:
    EscapeCharTable(){
        m_CharTable.resize(MAX_CHAR_VALUE);
        
        for(size_t i = 0; i < MAX_CHAR_VALUE; ++i){
            char buf[MAX_ESCAPED_LEN];
            // Unfortunately.. gcc 12.1 still has no std::format
            snprintf(buf, MAX_ESCAPED_LEN, "\\x%0X", i);
            m_CharTable[i] = std::string(buf);
        }
        // Whitespace should be escape as "%20?"
        m_CharTable[' '] = "%20";
    }

    std::string& escape(const char c){
        return m_CharTable[c];
    }
private:
    // Either it's %20 for whitespace or \xFF, max length is 5
    static constexpr size_t MAX_ESCAPED_LEN{5};
    static constexpr size_t MAX_CHAR_VALUE{255};
    std::vector<std::string> m_CharTable;
};

static EscapeCharTable g_escapeTable;
 
// Escaping all control char and whitespaces
void smallStringReplace(std::string& str)
{
  std::string::size_type pos = 0u;
  while(pos < str.size()){
    const auto c = str[pos];
    if(unlikely(std::iscntrl(c) || std::isspace(c))){
        std::string& newStr = g_escapeTable.escape(c);
        str.replace(pos, sizeof(c), newStr);// O(X * L)
        pos += newStr.length();
    }
    ++pos;
  }
}

void stringIterationReplace( std::string& s) {
    std::ostringstream oss;
    std::size_t pos = 0;
    std::size_t prevPos = 0;

    while (pos < s.size()) {
        const auto c = s[pos];
        if(unlikely(std::iscntrl(c) || std::isspace(c))){
            const std::string& newStr = g_escapeTable.escape(c);
            oss << s.substr(prevPos, pos - prevPos); // O(L)
            oss << newStr;
            pos+= sizeof(c);
        }        
        prevPos = pos;
        ++pos;
    }

    oss << s.substr(prevPos);
    s = oss.str();
}

std::string insertReplacingChars(const std::string input, const std::string toInsert,
    const int start ,const int interval){
    size_t insertPos = start;
    std::string copy = input;
    while(insertPos < copy.size()){
        copy.insert(insertPos, toInsert);
        
        // interval + added special
        insertPos += interval+toInsert.size();
    }
    return copy;
}

int main(){
    // https://stackoverflow.com/a/21710033/4123703
    // small str on 64bit platform. 22chars
    // Larger one
    const char* smallStr = "someone@somehewhereothersmaller.com";

    // Use Gutenberg project txt, https://www.gutenberg.org/cache/epub/2600/pg2600.txt
    // Insert 4\r 
    const char* longStr = "“Heavens! what a virulent attack!” replied the prince, not in theleast disconcerted by this reception. He had just entered, wearing anembroidered court uniform, knee breeches, and shoes, and had stars onhis breast and a serene expression on his flat face. He spoke in thatrefined French in which our grandfathers not only spoke but thought, andwith the gentle, patronizing intonation natural to a man of importancewho had grown old in society and at court. He went up to Anna Pávlovna,kissed her hand, presenting to her his bald, scented, and shining head,and complacently seated himself on the sofa.";
    static std::string output;
    std::string insertChar = "a";//"\r";
    int insertInterval = 10;
    int iteration = 2;
    int insertStart = rand()*5;
    {
    using unit = std::chrono::nanoseconds;
    auto totalDuration = 0;
    for(int i = 0; i < iteration; i++){
        std::string lstr = insertReplacingChars(longStr, insertChar,  insertStart + i, insertInterval);
        auto start = std::chrono::high_resolution_clock::now();
        smallStringReplace(lstr);
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<unit>(finish - start).count();
        output += lstr;
        std::cout << "smallStringReplace() long " << duration << "\n";
        totalDuration += duration;
    }
    std::cout << "smallStringReplace() long  string took total "
              << totalDuration
              << " nanoseconds\n";

    totalDuration = 0;
    for(int i = 0; i < iteration; i++){
        std::string sstr = insertReplacingChars(smallStr, insertChar, insertStart + i , insertInterval);
        auto start = std::chrono::high_resolution_clock::now();
        smallStringReplace(sstr);
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<unit>(finish - start).count();
        output += sstr;
        std::cout << "smallStringReplace() small " << duration << "\n";
        totalDuration += duration;
    }
    std::cout << "smallStringReplace() small string took total "
              << totalDuration
              << " nanoseconds\n";
    }
    {

    using unit = std::chrono::nanoseconds;
    auto totalDuration = 0;

    for(int i = 0; i < iteration; i++){
        std::string lstr = insertReplacingChars(longStr, insertChar,  insertStart + i, insertInterval);
        auto start = std::chrono::high_resolution_clock::now();
        stringIterationReplace(lstr);
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<unit>(finish - start).count();
        output += lstr;
        std::cout << "stringIterationReplace() long " << duration << "\n";
        totalDuration += duration;
    }
    std::cout << "stringIterationReplace() long  string took total "
              << totalDuration
              << " nanoseconds\n";
    totalDuration = 0;
    for(int i = 0; i < iteration; i++){
        std::string sstr = insertReplacingChars(smallStr, insertChar,  insertStart + i, insertInterval);
        auto start = std::chrono::high_resolution_clock::now();
        stringIterationReplace(sstr);
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<unit>(finish - start).count();
        output += sstr;
        std::cout << "stringIterationReplace() small " << duration << "\n";
        totalDuration += duration;
    }
    std::cout << "stringIterationReplace() small string took total "
              << totalDuration
              << " nanoseconds\n";
    }

}