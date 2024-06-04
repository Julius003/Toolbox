#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <random>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <cmath>

struct Edge{
    int from, to, weight;
};

struct Node{
    std::vector<int> neighbors;
};

std::string generateGraph(int node_count, int edge_percentage){
    std::string result = "";
    std::random_device rd;  // Seed generator
    std::default_random_engine generator(rd());  // Random number generator
    std::vector<Edge> edges;
    std::vector<Node> nodes;

    if(edge_percentage < 0 || edge_percentage > 100){
        edge_percentage = 50; // default to 50 percent
    }

    // Define the distribution range
    int edge_count = ((node_count) * (node_count-1))/ 2;
    edge_count *= edge_percentage;
    edge_count = edge_count / 100;
    edge_count -= node_count;

    std::uniform_int_distribution<int> weight_generator(1, 15);
    // Generate a random number

    Node start;
    start.neighbors.push_back(0);
    nodes.push_back(start);

    for(int i = 2; i <= node_count; ++i){
        // generate all nodes and connect each node with an already extisting one.
        // This way, tha graph is connected.
        int weight = weight_generator(generator);

        std::uniform_int_distribution<int> node(1, i - 1);
        int to = node(generator);

        Edge edge;
        edge.from = i;
        edge.to = to;
        edge.weight = weight;

        Edge reverse;
        reverse.from = to;
        reverse.to = i;
        reverse.weight = weight;

        edges.push_back(edge);
        edges.push_back(reverse);

        Node node_to_add;
        node_to_add.neighbors.push_back(to - 1);
        node_to_add.neighbors.push_back(i - 1);

        nodes.push_back(node_to_add);
        nodes[to - 1].neighbors.push_back(i - 1);
    }
    // Add the remaining edges in random fashion but don't make duplicates
    for(int i = 0; i < edge_count; ++i){
        // Select a node with open neighbors to connect to
        std::uniform_int_distribution<int> node_generator(0, node_count - 1); // 0 based index node
        int node = node_generator(generator);

        int safety = 0; // prevents the while loops from running endlessly
        while(nodes[node].neighbors.size() == node_count){
            node = (node + 1) % node_count;
            safety++;
            if(safety > node_count){
                goto getRes;
            }
        }
        // Select an open neighbor
        int neighbor = node_generator(generator);

        // select the next neighbor that is available
        safety = 0;
        while(std::find(nodes[node].neighbors.begin(), nodes[node].neighbors.end(), neighbor) != nodes[node].neighbors.end()){
            neighbor = (neighbor + 1)% node_count;
            safety++;
            if(safety > node_count){
                goto getRes;
            }
        }

        // create edge
        int weight = weight_generator(generator);

        Edge edge;
        edge.from = node + 1;
        edge.to = neighbor + 1;
        edge.weight = weight;

        Edge reverse;
        reverse.from = neighbor + 1;
        reverse.to = node + 1;
        reverse.weight = weight;

        edges.push_back(edge);
        edges.push_back(reverse);

        nodes[node].neighbors.push_back(neighbor);
        nodes[neighbor].neighbors.push_back(node);
    }
    goto getRes;

getRes:

    result += std::to_string(node_count);
    result += " " + std::to_string(edges.size());
    for(int i = 0; i < edges.size(); ++i){
        result += " " + std::to_string(edges[i].from);
        result += " " + std::to_string(edges[i].to);
        result += " " + std::to_string(edges[i].weight);
    }

    // Add the source and target node to the graph
    std::uniform_int_distribution<int> node_generator(1, node_count);
    int source = node_generator(generator);
    int target = node_generator(generator);

    if(source == target){
        target = (2 * target) % node_count + 1;
    }
    result += " " + std::to_string(source);
    result += " " + std::to_string(target);

    return result;
}

std::string cleanOutput(const std::string input) {
    std::string result;
    result.reserve(input.size()); // Reserve memory for efficiency

    // Copy characters except whitespace and newline characters
    for (char c : input) {
        if (!std::isspace(c) && c != '\n') {
            result.push_back(c);
        }
    }

    return result;
}

// This function is used to nicely display the result of the algorithms
std::vector<std::string> splitByNewline(const std::string& str) {
    std::vector<std::string> lines;
    std::istringstream stream(str);
    std::string line;
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    return lines;
}

void output_wrapped(const std::string& input, int max_width) {
    int remaining_width = max_width;
    std::string::size_type start = 0;
    std::string::size_type end;

    while (start < input.size()) {
        // Find the next line break position
        end = input.find('\n', start);

        // If the remaining width is less than the maximum width, output the remaining part of the string
        if (remaining_width <= max_width && input.size() - start <= remaining_width) {
            std::cout << input.substr(start) << std::endl;
            break;
        }

        // If no line break found or if the line break position exceeds the remaining width, find a suitable break point
        if (end == std::string::npos || end - start > remaining_width) {
            end = start + remaining_width;
            // Find the last whitespace within the remaining width to break the line
            while (end > start && !std::isspace(input[end])) {
                --end;
            }
        }

        // Output the line
        std::cout << input.substr(start, end - start) << std::endl;

        // Update start position for the next line
        start = end + 1;

        // Reset remaining width for the next line
        remaining_width = max_width;
    }
}


std::string exec(const std::string& cmd, const std::string& input) {
    int in_pipe[2];
    int out_pipe[2];
    pid_t pid;
    char buffer[128];
    std::string result;

    if (pipe(in_pipe) == -1 || pipe(out_pipe) == -1) {
        perror("pipe");
        return "";
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        return "";
    }

    if (pid == 0) {
        // Child process
        close(in_pipe[1]); // Close write end of in_pipe
        dup2(in_pipe[0], STDIN_FILENO); // Redirect stdin to in_pipe read end
        close(in_pipe[0]);

        close(out_pipe[0]); // Close read end of out_pipe
        dup2(out_pipe[1], STDOUT_FILENO); // Redirect stdout to out_pipe write end
        close(out_pipe[1]);

        execl("/bin/sh", "sh", "-c", cmd.c_str(), nullptr);
        perror("execl");
        _exit(1);
    } else {
        // Parent process
        close(in_pipe[0]); // Close read end of in_pipe
        write(in_pipe[1], input.c_str(), input.length()); // Write input to child
        close(in_pipe[1]); // Close write end of in_pipe

        close(out_pipe[1]); // Close write end of out_pipe
        ssize_t count;
        while ((count = read(out_pipe[0], buffer, sizeof(buffer))) > 0) {
            result.append(buffer, count);
        }
        close(out_pipe[0]); // Close read end of out_pipe

        waitpid(pid, nullptr, 0); // Wait for child to finish
    }

    return result;
}

void runTest(int test_id, const std::string& input, 
    const std::string& test_application, 
    const std::string& working_application, 
    std::vector<bool>& result_array, 
    std::vector<double>& performance,
    bool verbose) 
{
    std::string analysis = "";
    std::chrono::duration<double> test_duration, working_duration;
    //--------------------------------
    // Run the Test Graph Algorithm 
    //--------------------------------
    auto test_start = std::chrono::high_resolution_clock::now();
    std::string test_output = exec(test_application, input);
    auto test_end = std::chrono::high_resolution_clock::now();
    test_duration = test_end - test_start;

    //--------------------------------
    // Run the working Graph Algorithm 
    //--------------------------------
    auto working_start = std::chrono::high_resolution_clock::now();
    std::string working_output = exec(working_application, input);
    auto working_end = std::chrono::high_resolution_clock::now();
    working_duration = working_end - working_start;

    //--------------------------------
    // Test Summary
    //--------------------------------
    bool status = false;
    if(cleanOutput(working_output) == cleanOutput(test_output)){
        result_array[test_id - 1] = true;
        status = true;
    }

    performance[0] += test_duration.count();
    performance[1] += working_duration.count();

    if(verbose){
        std::cout << "             Test number " << test_id << "\n";
        std::cout << "Test Algorithm      |      Aproved Algorithm\n";
        std::cout << "--------------------------------------------\n";
        std::cout << test_duration.count() << "s                 " << working_duration.count() << "s\n";
        std::cout << "               Status: " << (status ? "✅" : "❌") << "\n";
        std::cout << "        Total duration: " << (int)((test_duration.count() + working_duration.count()) * 1000) << "ms\n";
        std::cout << "--------------------------------------------\n";
        // std::cout << "Output Test: " << std::endl;
        // std::cout << test_output << std::endl;

        // std::cout << "Output Aproved: " << std::endl;
        // std::cout << working_output << std::endl;

        std::vector<std::string> test_output_lines = splitByNewline(test_output);
        std::vector<std::string> working_output_lines = splitByNewline(working_output);

        size_t maxLines = std::max(working_output_lines.size(), test_output_lines.size());

        for (size_t i = 0; i < maxLines; ++i) {
            if (i < test_output_lines.size()) {
                std::cout << std::left << std::setw(30) << test_output_lines[i];
            } else {
                std::cout << std::left << std::setw(30) << "";
            }

            if (i < working_output_lines.size()) {
                std::cout << working_output_lines[i];
            }

            std::cout << std::endl;
        }

        std::cout << std::endl;
        //std::cout << "Exit Codes: " << test_return_code << " <--> " << working_return_code << std::endl;
        // std::cout << "With Input: " << std::endl;
        // output_wrapped(input, 45);
        std::cout << "--------------------------------------------\n\n";
    }
}

void displayHelp() {
    std::cout << "Usage: graphtest [options] <file1> <file2>\n";
    std::cout << "\n";

    std::cout << "Required Parameters:\n";
    std::cout << "  file1    Path to the compiled file to test.\n";
    std::cout << "  file2    Path to the compiled file that already works.\n";

    std::cout << "\n";
    std::cout << "Options:\n";
    std::cout << "  --max <value>             The maximum number of nodes in the graphs that get generated\n";
    std::cout << "  --min <value>             The minimum number of nodes in the graphs that get generated\n";
    std::cout << "  -h, --help                Show this help message\n";
    std::cout << "  -t, --test_count <value>  The number of graphs that get generated\n";
    std::cout << "  -V, --verbose             Output details about each individual test\n";

    std::cout << std::endl;
    std::cout << "Defaults:\n";
    std::cout << "  --min           10\n";
    std::cout << "  --max           30\n";
    std::cout << "  --test_count    5\n";
    
    // Add more options as needed
    std::cout << std::endl;
    std::cout << "Example:\n";
    std::cout << "  graphtest ./bfs_adj ./bfs --min 20 --max 100 -t 5 --verbose\n";
}

void displayInformation(int max_node_count, int min_node_count, int test_count){
    std::cout << std::endl;
    std::cout << "--------------------------------------------------------\n";
    std::cout << "|              Testing Graph Algorithms                |\n";
    std::cout << "|                 against each other                   |\n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << "Input Parameters:\n";
    std::cout << "  Number of Tests on graphs with " << min_node_count << " nodes:   " << test_count << std::endl;
    std::cout << "  Number of Tests on graphs with " << max_node_count << " nodes:   " << test_count << std::endl;
    std::cout << "========================================================\n\n";
}

int main(int argc, char** argv){
    if(argc == 1){
        displayHelp();
        return 0;
    }
    int max_node_count = 30;
    int min_node_count = 10;
    int test_count = 5;
    int sparse_test_index = 1;
    bool verbose = false;
    bool show_input = false;

    std::vector<std::string> parameters;
    for (int i = 1; i < argc; ++i) {
        bool required = true;
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            displayHelp();
            return 0;
        } 
        else if (arg == "--max") {
            required = false;
            max_node_count = std::atoi(argv[i + 1]);
            i++;
        }
        else if (arg == "--min") {
            required = false;
            min_node_count = std::atoi(argv[i + 1]);
            i++;
        }
        else if (arg == "-t" || arg == "--test") {
            required = false;
            test_count = std::atoi(argv[i + 1]);
            sparse_test_index = (test_count / 2) - 1;
            i++;
        }
        else if(arg == "-v" || arg == "--verbose"){
            verbose = true;
        }
        else if(arg == "-s" || arg == "--show_input"){
            show_input = true;
        }

        if(required){
            parameters.push_back(argv[i]);
        }
    }
    displayInformation(max_node_count, min_node_count, test_count);

    std::vector<bool> test_results(test_count * 2, false);
    std::vector<double> performance_sum(test_count * 2, 0);
    std::vector<std::string> test_analysis(test_count * 2, "");
    
    std::cout << "Running Tests...\n";
    // Testing on small graphs
    for(int i = 0; i <= sparse_test_index; ++i){
        std::string graph = generateGraph(min_node_count, 10);
        if(show_input){
            std::cout << "Running Algorithms on the following input: " << std::endl;
            output_wrapped(graph, 45);
            std::cout << "-------------------------------------------\n";
        }
        runTest(i + 1, graph, parameters[0], parameters[1], test_results, performance_sum, verbose);
    }

    for(int i = sparse_test_index + 1; i < test_count; ++i){
        std::string graph = generateGraph(min_node_count, 90);
        if(show_input){
            std::cout << "Running Algorithms on the following input: " << std::endl;
            output_wrapped(graph, 45);
            std::cout << "-------------------------------------------\n";
        }
        runTest(i + 1, graph, parameters[0], parameters[1], test_results, performance_sum, verbose);
    }
    // Testing on big graphs
    for(int i = 0; i <= sparse_test_index; ++i){
        std::string graph = generateGraph(max_node_count, 10);
        if(show_input){
            std::cout << "Running Algorithms on the following input: " << std::endl;
            output_wrapped(graph, 45);
            std::cout << "-------------------------------------------\n";
        }
        runTest(test_count + i + 1, graph, parameters[0], parameters[1], test_results, performance_sum, verbose);
    }

    for(int i = sparse_test_index + 1; i < test_count; ++i){
        std::string graph = generateGraph(max_node_count, 90);
        if(show_input){
            std::cout << "Running Algorithms on the following input: " << std::endl;
            output_wrapped(graph, 45);
            std::cout << "-------------------------------------------\n";
        }
        runTest(test_count + i + 1, graph, parameters[0], parameters[1], test_results, performance_sum, verbose);
    }
    std::cout << "Done...\n";
    std::cout << std::endl;
    std::cout << "    Test Analysis:\n";
    std::cout << "------------------------" << std::endl;
    std::cout << "\nSparse Graphs: " << std::endl;
    std::cout << min_node_count << " Nodes: ";
    for(int i = 0; i <= sparse_test_index; ++i){
        if(test_results[i]) std::cout << "✅" << " ";
        else std::cout << "❌" << " ";
    }
    std::cout << std::endl;
    std::cout << max_node_count << " Nodes: ";
    for(int i = sparse_test_index + 1; i < test_count; ++i){
        if(test_results[i]) std::cout << "✅" << " ";
        else std::cout << "❌" << " ";
    }
    std::cout << std::endl;
    std::cout << "\nDense Graphs: " << std::endl;
    std::cout << min_node_count << " Nodes: ";
    for(int i = test_count; i <= test_count + sparse_test_index; ++i){
        if(test_results[i]) std::cout << "✅" << " ";
        else std::cout << "❌" << " ";
    }
    std::cout << std::endl;
    std::cout << max_node_count << " Nodes: ";
    for(int i = test_count + sparse_test_index + 1; i < test_count * 2; ++i){
        if(test_results[i]) std::cout << "✅" << " ";
        else std::cout << "❌" << " ";
    }
    std::cout << "\n\n";
    std::cout << "Time taken: " << std::round((performance_sum[0] + performance_sum[1])* 10)/10 << "s or " << (int)((performance_sum[0] + performance_sum[1])*1000) << "ms\n";
    std::cout << parameters[0].substr(2) << ": " << (int)(performance_sum[0]*1000) << "ms\n";
    std::cout << parameters[1].substr(2) << ": " << (int)(performance_sum[1]*1000) << "ms\n\n";
    int relative_performance = std::round((performance_sum[1] / performance_sum[0]) * 100) - 100;

    if(relative_performance >= 0){
        std::cout << parameters[0].substr(2) << " was " << relative_performance << " % faster than " << parameters[1].substr(2) << std::endl;
    }
    else {
        std::cout << parameters[0].substr(2) << " was " << -relative_performance << " % slower than " << parameters[1].substr(2) << std::endl;
    }
    std::cout << std::endl;
}