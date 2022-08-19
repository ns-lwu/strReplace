#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <string_view>
#include <chrono>
#include <tuple>


static void escape(void *p){
    asm volatile("" : : "g"(p) : "memory");
}
static void clobber(){
    asm volatile("" : : : "memory");
}
 
void smallStringReplace(std::string& str,
               const char org,
               const std::string& newStr)
{
  std::string::size_type pos = 0u;
  while((pos = str.find(org, pos)) != std::string::npos){
     str.replace(pos, sizeof(org), newStr);// O(X * L)
     pos += newStr.length();
  }
}

void stringIterationReplace( 
    std::string& str,
    std::vector<std::tuple<char, std::string>>& org,
    std::string const& newStr
) {
    size_t pos = 0;
    while(pos < str.size()){
        for(auto& tup: org){
            if(str[pos] == get<0>(tup)){
                str.replace(pos, sizeof(get<0>(tup)), get<1>(tup));// O(X * L)    
                pos += get<1>(tup).length();
            }
            ++pos;            
        }
    }
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
    std::string insertChar = "\r\n"; //"\r";
    std::vector<std::tuple<char, std::string>> toReplace = 
        {std::make_tuple('\r', "\\r"), 
         std::make_tuple('\n', "\\n")};
    int insertInterval = 10;
    int iteration = 2;
    int insertStart = 7;
    {
    using unit = std::chrono::nanoseconds;
    auto totalDuration = 0;
    for(int i = 0; i < iteration; i++){
        std::string lstr = insertReplacingChars(longStr, insertChar,  insertStart + i, insertInterval);
        auto start = std::chrono::high_resolution_clock::now();
        smallStringReplace(lstr, get<0>(toReplace[0]), get<1>(toReplace[0]));
        smallStringReplace(lstr,get<0>(toReplace[1]), get<1>(toReplace[1]));
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
        smallStringReplace(sstr, get<0>(toReplace[0]), get<1>(toReplace[0]));
        smallStringReplace(sstr,get<0>(toReplace[1]), get<1>(toReplace[1]));
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
        stringIterationReplace(lstr, toReplace, "\\r");
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
        stringIterationReplace(sstr, toReplace, "\\r");
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