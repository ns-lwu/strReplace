#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <string_view>
#include <chrono>
#include <tuple>
#include <unordered_map>


static void escape(void *p){
    asm volatile("" : : "g"(p) : "memory");
}
static void clobber(){
    asm volatile("" : : : "memory");
}
 

void stringIterationReplaceVec( 
    std::string& str,
    std::vector<std::tuple<char, std::string>>& org) {
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

void stringIterationReplaceMap( 
    std::string& str,
    std::unordered_map<char, std::string>& map) {
    size_t pos = 0;
    while(pos < str.size()){
        char c = str[pos];
        if(map.count(c) == 1){
            str.replace(pos, sizeof(c), map[c]);// O(X * L)    
            pos += map[c].length();
        }
        ++pos;            
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
    std::vector<std::tuple<char, std::string>> toReplace{
        std::make_tuple('\r', "\\r"), 
         std::make_tuple('\n', "\\n")};
    std::unordered_map<char, std::string> toReplaceMap{{'\r', "\\r"}, {'\n', "\\n"}};

    int insertInterval = 10;
    int iteration = 2;
    int insertStart = 7;
    { // test for vector

    using unit = std::chrono::nanoseconds;
    auto totalDuration = 0;

    for(int i = 0; i < iteration; i++){
        std::string lstr = insertReplacingChars(longStr, insertChar,  insertStart + i, insertInterval);
        auto start = std::chrono::high_resolution_clock::now();
        stringIterationReplaceVec(lstr, toReplace);
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<unit>(finish - start).count();
        output += lstr;
        std::cout << "stringIterationReplaceVec() long string " << duration << "\n";
        totalDuration += duration;
    }
    std::cout << "stringIterationReplaceVec() long string took total "
              << totalDuration
              << " nanoseconds\n";
    totalDuration = 0;
    for(int i = 0; i < iteration; i++){
        std::string sstr = insertReplacingChars(smallStr, insertChar,  insertStart + i, insertInterval);
        auto start = std::chrono::high_resolution_clock::now();
        stringIterationReplaceVec(sstr, toReplace);
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<unit>(finish - start).count();
        output += sstr;
        std::cout << "stringIterationReplaceVec() small string " << duration << "\n";
        totalDuration += duration;
    }
    std::cout << "stringIterationReplaceVec() small string took total "
              << totalDuration
              << " nanoseconds\n";
    }
    { //  test for map

    using unit = std::chrono::nanoseconds;
    auto totalDuration = 0;

    for(int i = 0; i < iteration; i++){
        std::string lstr = insertReplacingChars(longStr, insertChar,  insertStart + i, insertInterval);
        auto start = std::chrono::high_resolution_clock::now();
        stringIterationReplaceMap(lstr, toReplaceMap);
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<unit>(finish - start).count();
        output += lstr;
        std::cout << "stringIterationReplaceMap() long " << duration << "\n";
        totalDuration += duration;
    }
    std::cout << "stringIterationReplaceMap() long  string took total "
              << totalDuration
              << " nanoseconds\n";
    totalDuration = 0;
    for(int i = 0; i < iteration; i++){
        std::string sstr = insertReplacingChars(smallStr, insertChar,  insertStart + i, insertInterval);
        auto start = std::chrono::high_resolution_clock::now();
        stringIterationReplaceMap(sstr, toReplaceMap);
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