#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <string_view>
#include <chrono>
#include <fstream>

// Borrowed from https://youtu.be/nXaxk27zwlk?t=2671
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

std::vector<std::string> testCases(){
    std::ifstream ifs("testcases.txt");
    std::vector<std::string> cases;

    for (std::string line; std::getline(ifs, line); ) 
    {
        cases.push_back(line);
    }
    return cases;
}

int main(){
    // https://stackoverflow.com/a/21710033/4123703
    // small str on 64bit platform. 22chars
    // Larger one
    std::string orgStr = "someo\rne@somehewhereot\rhersmaller.com";
    static std::string output; // try to not get string optimized out
    // std::vector<std::string> smallStr{
    //     "someo\rne@somehewhereot\rhersmaller.com",
    //     "another\rthingelse@aaazzzie\rusw.comjlkwjerewkru",
    //     "AADSGSDAFGA\rne@ZZZZZZZ\rFGFFFFFFFF",
    //     "QOJMVUWNVIU\rne@SDKJCWENM\rSDFKJSDFMNWENCUWE",
    //     "XCNVXCMVN\rne@QWEUTRQOWYQWEYQW\rKSDFJSDFI"};
    std::vector<std::string> smallStr = testCases();
    clobber();
    auto pos = orgStr.find("\r", 0);
    std::cout << pos;
    if(pos == std::string::npos){std::cout <<"NOPS\n";}

    std::string insertChar = "\r";
    int iteration = 5;
    using unit = std::chrono::nanoseconds;
    auto totalDuration = 0;
    for(int i = 0; i < iteration; i++){
        std::string copy = orgStr;
        copy[i] = 'Z';
        smallStr.push_back(copy);
    }

    for(int i = 0; i < iteration; i++){
        std::string sstr;
        escape(&sstr);
        // sstr = smallStr[i]; 
        sstr = insertReplacingChars(smallStr[i], insertChar,  rand()*10, 7);
        auto start = std::chrono::high_resolution_clock::now();
        smallStringReplace(sstr, "\r", "\\r");
        auto finish = std::chrono::high_resolution_clock::now();
        clobber();
        auto duration = std::chrono::duration_cast<unit>(finish - start).count();
        std::cout << "smallStringReplace() small " << duration << "\n";
        output += sstr;
        totalDuration += duration;
    }

    std::cout << output << "\n";
}