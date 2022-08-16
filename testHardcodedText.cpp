#include <string>
#include <iostream>
#include <sstream>
#include <string_view>
#include <chrono>

static void escape(void *p){
    asm volatile("" : : "g"(p) : "memory");
}
static void clobber(){
    asm volatile("" : : : "memory");
}
 
void smallStringReplace(std::string& str,
               const std::string& oldStr,
               const std::string& newStr)
{
  std::string::size_type pos = 0u;
  while((pos = str.find(oldStr, pos)) != std::string::npos){
     str.replace(pos, oldStr.length(), newStr);
     pos += newStr.length();
  }
}

void stringStreamReplace( 
    std::string& s,
    std::string const& oldStr,
    std::string const& newStr
) {
    std::ostringstream oss;
    std::size_t pos = 0;
    std::size_t prevPos;

    while (true) {
        prevPos = pos;
        pos = s.find(oldStr, pos);
        if (pos == std::string::npos)
            break;
        oss << s.substr(prevPos, pos - prevPos);
        oss << newStr;
        pos += oldStr.size();
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
    std::string insertChar = "\r";
    int iteration = 1;
    {
    using unit = std::chrono::nanoseconds;
    auto totalDuration = 0;
    for(int i = 0; i < iteration; i++){
        std::string sstr = insertReplacingChars(smallStr, insertChar,  rand()*5+i, 7);
        auto start = std::chrono::high_resolution_clock::now();
        smallStringReplace(sstr, "\r", "\\r");
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<unit>(finish - start).count();
        output += sstr;
        std::cout << "smallStringReplace() small " << duration << "\n";
        totalDuration += duration;
    }
    std::cout << "smallStringReplace() small string took total "
              << totalDuration
              << " nanoseconds\n";
    totalDuration = 0;
    for(int i = 0; i < iteration; i++){
        std::string lstr = insertReplacingChars(longStr, insertChar,  rand()*5+i, 7);
        auto start = std::chrono::high_resolution_clock::now();
        smallStringReplace(lstr, "\r", "\\r");
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<unit>(finish - start).count();
        output += lstr;
        std::cout << "smallStringReplace() long " << duration << "\n";
        totalDuration += duration;
    }
    std::cout << "smallStringReplace() long  string took total "
              << totalDuration
              << " nanoseconds\n";
    }
    {

    using unit = std::chrono::nanoseconds;
    auto totalDuration = 0;
    for(int i = 0; i < iteration; i++){
        std::string sstr = insertReplacingChars(smallStr, insertChar,  rand()*5+i, 7);
        auto start = std::chrono::high_resolution_clock::now();
        stringStreamReplace(sstr, "\r", "\\r");
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<unit>(finish - start).count();
        output += sstr;
        std::cout << "stringStreamReplace() small " << duration << "\n";
        totalDuration += duration;
    }
    std::cout << "stringStreamReplace() small string took total "
              << totalDuration
              << " nanoseconds\n";
    totalDuration = 0;
    for(int i = 0; i < iteration; i++){
        std::string lstr = insertReplacingChars(longStr, insertChar,  rand()*5+i, 7);
        auto start = std::chrono::high_resolution_clock::now();
        stringStreamReplace(lstr, "\r", "\\r");
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<unit>(finish - start).count();
        output += lstr;
        std::cout << "stringStreamReplace() long " << duration << "\n";
        totalDuration += duration;
    }
    std::cout << "stringStreamReplace() long  string took total "
              << totalDuration
              << " nanoseconds\n";
    }
}