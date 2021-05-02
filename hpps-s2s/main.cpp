#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class ConvertedVariable {
public:
    string name;
    string line;
    string integerBits;
    string fractionalBits;
};

class FunctionCall {
public:
    string isBuiltIn;
    int line;
    int column;
};

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

// source/source.cpp -> source file to transform
// conversion -> file that contains conversion metadata from TAFFO
int main() {

    const string SOURCE_PATH = "/Users/cosimosguanci/Desktop/TAFFO_S2S/source/source.cpp";
    const string SOURCE_PATH_OPTIMIZED = "/Users/cosimosguanci/Desktop/TAFFO_S2S/source/source_op.cpp";

    const string DECLARATIONS_PATH = "/Users/cosimosguanci/Desktop/TAFFO_S2S/declarations";
    const string CONVERSION_PATH = "/Users/cosimosguanci/Desktop/TAFFO_S2S/conversion";

    vector<ConvertedVariable> convertedVariables;
    vector<FunctionCall> functions;

    vector<string> lines;
    std::string line;

    ifstream src;
    ofstream dst;

    src.open(SOURCE_PATH, ios::in | ios::binary);
    dst.open(SOURCE_PATH_OPTIMIZED, ios::out | ios::binary);
    dst << src.rdbuf(); // copy the source file to the newly created optimized one

    src.close();
    dst.close();

    std::fstream sourceOp(SOURCE_PATH_OPTIMIZED);

    std::string codeLine = "#include \"fixed_point.hpp\"";
    lines.push_back(codeLine);

    while (std::getline(sourceOp, codeLine))
    {
        lines.push_back(codeLine);
    }

    std::ofstream ofStreamOptimized (SOURCE_PATH_OPTIMIZED);

    //sourceOp.clear();
    //sourceOp.seekg(0);

    for(const auto& value: lines) {
        //sourceOp<<value<<endl;
        ofStreamOptimized<<value<<endl;
    }

    // read declarations file
    std::ifstream declFile(DECLARATIONS_PATH);
    while (std::getline(declFile, line))
    {

        ConvertedVariable variable;

        string variableName;
        string variableLine;

        string integerPart; // number of bits
        string fractionalPart;

        size_t index;

        index = line.find(' ');

        variableName = line.substr(0, index);
        index++;

        line = line.substr(index);
        index = line.find(' ');

        variableLine = line.substr(0, index);

        char endCharacter = ';';

        if(line.find("function") != std::string::npos) {
            if(lines.at(stoi(variableLine) + 2).find('{') != std::string::npos) {
                endCharacter = ' ';
            }
            else {
                endCharacter = ',';

            }
            replace(line, " function", "");
        }

        index++;

        line = line.substr(index);
        index = line.find(' ');

        integerPart = line.substr(0, index);

        index++;
        line = line.substr(index);

        fractionalPart = line.substr(0);


        string oldLineOfCode = lines.at(stoi(variableLine));

        string newLineOfCode = "\tfixed_point_t<" + integerPart + "," + fractionalPart + "> " + variableName + endCharacter;

        lines[stoi(variableLine)] = newLineOfCode;


        variable.name = variableName;
        variable.line = variableLine;
        variable.integerBits = integerPart;
        variable.fractionalBits = fractionalPart;

        convertedVariables.push_back(variable);
    }

    //sourceOp.clear();
    //sourceOp.seekg(0);

    ofStreamOptimized = ofstream (SOURCE_PATH_OPTIMIZED);
    for(const auto& value: lines) {
        //sourceOp<<value<<endl;
        ofStreamOptimized<<value<<endl;
    }

    // read conversion file
    std::ifstream convFile(CONVERSION_PATH);

    // first, I get all the line of code where there is a call and I differentiate between built in and not built in functions

    while (std::getline(convFile, line)) {
        if (line.find("call") != std::string::npos) {
            size_t index;

            index = line.find(' ');

            int functionLine = stoi(line.substr(0, index));


            index++;

            line = line.substr(index);
            index = line.find(' ');

            int functionColumn = stoi(line.substr(0, index));

            FunctionCall functionCall;

            if (line.find("NOT-BUILT-IN") != std::string::npos) {
                functionCall.isBuiltIn = "NOT-BUILT-IN";
            }
            else {
                functionCall.isBuiltIn = "BUILT-IN";
            }

            functionCall.line = functionLine;
            functionCall.column = functionColumn;
            functions.push_back(functionCall);
        }
    }

    /// no need to read conversion file again, I have what I need!

    for(const auto& funCall: functions) {
        string lineOfCode = lines[funCall.line];
        lineOfCode = lineOfCode.substr(funCall.column-1, (lineOfCode.size() - funCall.column) + 1);
        string functionName = lineOfCode.substr(0, lineOfCode.find_first_of('('));

        lineOfCode = lineOfCode.substr(lineOfCode.find_first_of('(') + 1, (lineOfCode.size() - lineOfCode.find_first_of('(')) + 1);

        vector<string> parameters;
        // list all the parameters
        int indexOfClosedPar = lineOfCode.find_first_of(')');
        int indexOfComma = lineOfCode.find_first_of(','); // -1 if not present

        if(indexOfComma == -1) {
            // 1 parameter
            string param = lineOfCode.substr(0, indexOfClosedPar);
            parameters.push_back(param);
        }
        else {
            while(indexOfComma != -1) {
                string param = lineOfCode.substr(0, indexOfComma);
                parameters.push_back(param);
                lineOfCode = lineOfCode.substr(indexOfComma + 1);
                indexOfComma = lineOfCode.find_first_of(',');
            }

            string param = lineOfCode.substr(0, lineOfCode.find_first_of(')'));
            parameters.push_back(param);

        }

        if(funCall.isBuiltIn == "BUILT-IN") {
            int i = 0;

            string previousFunctionCallString = functionName + "(";

            for(const auto& param: parameters) {
                if(i != parameters.size() - 1) {
                    previousFunctionCallString = previousFunctionCallString + param + ",";
                }
                else {
                    previousFunctionCallString = previousFunctionCallString + param + ")";
                }
                i++;
            }

            i = 0;
            for(const auto& param: parameters) {
                for(const auto& var: convertedVariables) {
                    if(var.name == param) {
                        parameters[i] = param + ".getValueF()";
                    }
                }

                i++;
            }

            string functionCallString = functionName + "(";

            i = 0;
            for(const auto& param: parameters) {
                if(i != parameters.size() - 1) {
                    functionCallString = functionCallString + param + ",";
                }
                else {
                    functionCallString = functionCallString + param + ")";
                }
                i++;
            }

            replace(lines[funCall.line], previousFunctionCallString, functionCallString);

        }
        /*else if(funCall.isBuiltIn == "NOT-BUILT-IN"){
            // need to find the function and work on it
            int j = 1;
            for(const auto& oldLine: lines) {
                if(oldLine.find(" " + functionName + "(") != std::string::npos) {
                    bool foundInCalls = false;
                    for(const auto& calls: functions) {
                        if(calls.line == j) {
                            foundInCalls = true;
                            break;
                        }
                    }

                    if(!foundInCalls) {
                        // then it is start of the function definition
                    }
                }
            }
        }*/
    }

    //sourceOp.clear();
    //sourceOp.seekg(0);

    for(const auto& convertedVariable: convertedVariables) {
        int i = 0;
        for(const auto& value: lines) {
            if(lines[i].find("return " + convertedVariable.name) != std::string::npos) {
                replace(lines[i], "return " + convertedVariable.name, "return " + convertedVariable.name + ".getValueF()");
            }
            i++;
        }
    }

    ofStreamOptimized = ofstream (SOURCE_PATH_OPTIMIZED);

    for(const auto& value: lines) {
        //sourceOp<<value<<endl;
        ofStreamOptimized<<value<<endl;
    }





    printf("done");

    ///
    /*convFile.clear();
    convFile.seekg(0);

    while (std::getline(convFile, line)) {

        string convertedLine;
        string convertedColumn;

        string convertedOpcode;
        string isFunctionConvertedBuiltIn;

        size_t index;

        index = line.find(' ');

        convertedLine = line.substr(0, index);
        index++;

        line = line.substr(index);
        index = line.find(' ');

        convertedColumn = line.substr(0, index);
        index++;

        line = line.substr(index);
        index = line.find(' ');

        convertedOpcode = line.substr(0, index);

        if(convertedOpcode == "call") { // already handled
            continue;
        }

        index++;
        line = line.substr(index);

        isFunctionConvertedBuiltIn = line.substr(0);

        printf("a");

        string lineOfCode = lines.at(stoi(convertedLine));


        bool changed = false;

        if(functions[stoi(convertedLine)].isBuiltIn == "BUILT-IN") {
            for(const auto& var: convertedVariables) {

                if(lineOfCode.find('(' + var.name + ')') != std::string::npos) {
                    replace(lineOfCode, '(' + var.name + ')', '(' + var.name + ".getValueF())"); // TODO handle also double etc (OLD TYPE)
                    changed = true;
                    break;
                }
                else if(lineOfCode.find('(' + var.name + ',') != std::string::npos) {
                    replace(lineOfCode, '(' + var.name + ',', '(' + var.name + ".getValueF(),"); // TODO handle also double etc (OLD TYPE)
                    changed = true;
                    break;
                }
                else if(lineOfCode.find(", " + var.name + ',') != std::string::npos) {
                    replace(lineOfCode, ", " + var.name + ',', ", " + var.name + ".getValueF(),"); // TODO handle also double etc (OLD TYPE)
                    changed = true;
                    break;
                }
                else if(lineOfCode.find(',' + var.name + ',') != std::string::npos) {
                    replace(lineOfCode, ',' + var.name + ',', ", " + var.name + ".getValueF(),"); // TODO handle also double etc (OLD TYPE)
                    changed = true;
                    break;
                }
                else if(lineOfCode.find(", " + var.name + ')') != std::string::npos) {
                    replace(lineOfCode, ", " + var.name + ')', ", " + var.name + ".getValueF())"); // TODO handle also double etc (OLD TYPE)
                    changed = true;
                    break;
                }
                else if(lineOfCode.find(',' + var.name + ')') != std::string::npos) {
                    replace(lineOfCode, ',' + var.name + ')', ", " + var.name + ".getValueF())"); // TODO handle also double etc (OLD TYPE)
                    changed = true;
                    break;
                }

            }
        }

        else if(functions[stoi(convertedLine)].isBuiltIn == "NOT-BUILT-IN"){

            /// 1 - find function name
            /// 2 - find function in code
            /// 3 - change type of parameter (need to know if the parameter is the first/second etc)
            for(const auto& var: convertedVariables) {

                string functionName;
                int parameterPos;

                if(lineOfCode.find('(' + var.name + ',') != std::string::npos) {
                    replace(lineOfCode, '(' + var.name + ',', '(' + var.name + ".getValueF(),"); // TODO handle also double etc (OLD TYPE)
                    changed = true;
                    break;
                }
                else if(lineOfCode.find(", " + var.name + ',') != std::string::npos) {
                    replace(lineOfCode, ", " + var.name + ',', ", " + var.name + ".getValueF(),"); // TODO handle also double etc (OLD TYPE)
                    changed = true;
                    break;
                }
                else if(lineOfCode.find(',' + var.name + ',') != std::string::npos) {
                    replace(lineOfCode, ',' + var.name + ',', ", " + var.name + ".getValueF(),"); // TODO handle also double etc (OLD TYPE)
                    changed = true;
                    break;
                }
                else if(lineOfCode.find(", " + var.name + ')') != std::string::npos) {
                    replace(lineOfCode, ", " + var.name + ')', ", " + var.name + ".getValueF())"); // TODO handle also double etc (OLD TYPE)
                    changed = true;
                    break;
                }
                else if(lineOfCode.find(',' + var.name + ')') != std::string::npos) {
                    replace(lineOfCode, ',' + var.name + ')', ", " + var.name + ".getValueF())"); // TODO handle also double etc (OLD TYPE)
                    changed = true;
                    break;
                }

            }
        }


        if(changed) {
            lines[stoi(convertedLine)] = lineOfCode;

            sourceOp.clear();
            sourceOp.seekg(0);

            for(const auto& value: lines) {
                sourceOp<<value<<endl;
            }
        }


    }

    printf("done");*/



    // read conversion file
    /*std::ifstream infile(CONVERSION_PATH);

    while (std::getline(infile, line))
    {
        size_t index;
        size_t index2;

        index = line.find(DELIMITER) + 1;

        line = line.substr(index);

        index2 = line.find(DELIMITER);

        string newType = line.substr(0, index2);

        line = line.substr(index2+1);

        ///

        index = line.find(DELIMITER) + 1;

        line = line.substr(index);

        index2 = line.find(DELIMITER);

        string oldType = line.substr(0, index2);

        line = line.substr(index2+1);

        ///

        index = line.find(DELIMITER) + 1;

        line = line.substr(index);

        index2 = line.find(DELIMITER);

        string sourceCodeLine = line.substr(0, index2);

        line = line.substr(index2+1);

        ///

        index = line.find(DELIMITER) + 1;

        line = line.substr(index);

        index2 = line.find(DELIMITER);

        string sourceCodeCol = line.substr(0, index2);

        line = line.substr(index2+1);

        ///

        index = line.find(DELIMITER) + 1;

        line = line.substr(index);

        index2 = line.find(DELIMITER);

        string opcode = line.substr(0, index2);

        line = line.substr(index2+1);

        ///

        lines.clear();

        /// reload all lines of code

        sourceOp.clear();
        sourceOp.seekg(0);

        while (std::getline(sourceOp, codeLine))
        {
            lines.push_back(codeLine);
        }

        string lineOfCode = lines.at(stoi(sourceCodeLine));

        if(lineOfCode.find("__attribute__((annotate(") != std::string::npos) {
            // DECLARATION
            if(oldType == "i8*")
                continue;

            newType = newType.substr(1);
            index = newType.find('_');

            string integerPart = newType.substr(0, index);
            string fractionalPart = newType.substr(index+1, newType.find('f') - (index+1));

            if(oldType.find('*') != std::string::npos) {
                oldType = oldType.substr(0, oldType.size()-1);
            }



            index = lineOfCode.find(oldType);
            index2 = lineOfCode.find('_');
            lineOfCode.erase(index2, (index-index2) + oldType.size());
            //lineOfCode.replace(lineOfCode.begin() + index2,lineOfCode.begin()+((index-index2) + oldType.size())," ");             // "replace phrase!!!"     (3)


            string newTypeToBeInserted = "fixed_point_t<" + integerPart + "," + fractionalPart + "> ";

            lineOfCode.insert(index - (((index-index2) + oldType.size()) - index2), newTypeToBeInserted);

            lines.at(stoi(sourceCodeLine)) = lineOfCode;

            sourceOp.clear();
            sourceOp.seekg(0);

            for(const auto& value: lines) {
                sourceOp<<value<<endl;
            }

        }
        else if(opcode == "store") {
            // Something like myVar = ... , with myVar being the variable converted from float2fix

            newType = newType.substr(1);
            index = newType.find('_');

            string integerPart = newType.substr(0, index);
            string fractionalPart = newType.substr(index+1, newType.find('f') - (index+1));

            if(oldType.find('*') != std::string::npos) {
                oldType = oldType.substr(0, oldType.size()-1);
            }

            index = lineOfCode.find('=');
            index2 = lineOfCode.find(';');

            string sub1 = lineOfCode.substr(0, index + 1);

            string sub2 = lineOfCode.substr(index + 2, index2 - (index+2));

            lineOfCode = sub1 + "fixed_point_t(" + sub2 + ");"; // <" + integerPart + ", " + fractionalPart + ">

            lines.at(stoi(sourceCodeLine)) = lineOfCode;

            sourceOp.clear();
            sourceOp.seekg(0);

            for(const auto& value: lines) {
                sourceOp<<value<<endl;
            }


        }






    }*/

    return 0;
}
