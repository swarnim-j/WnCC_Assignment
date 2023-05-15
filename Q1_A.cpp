#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <algorithm>
#include <set>

using namespace std;

/*
The solution uses the maximum bipartite matching algorithm to find the maximum number of projects
that can be completed with the available participants.

The isInteger function checks if a given string represents an integer by iterating through each
character in the string and checking if it is a digit.

The bpm function implements the maximum bipartite matching algorithm using a recursive approach.
It takes a boolean graph bpGraph representing the bipartite graph, a starting vertex u, a boolean
array seen representing the set of visited vertices, an integer array matchR representing the matching
of the right set of vertices, and the number of vertices M and N. It checks all vertices in the right
set that are connected to u in the left set and tries to find an augmenting path recursively. If an
augmenting path is found, it updates the matching and returns true. Otherwise, it returns false.

The maxBPM function initializes the matching array matchR and calls the bpm function for each vertex in
the left set, updating the maximum matching if a new augmenting path is found.

The completedProjects function takes the number of participants N, the number of projects M, two maps U
and V representing the skills of the participants and projects, respectively. It generates all possible
permutations of the projects and tries to match each permutation with the available participants using
the maximum bipartite matching algorithm. It then keeps track of the maximum number of projects that
can be completed and returns this value.

The main function reads the input data, calls the completedProjects function, and outputs the result.

The code currently does not account for cases where a mentee can be assigned a project.
*/

bool isInteger(string str)
{
    for (char c : str)
    {
        if (!isdigit(c))
        {
            return false;
        }
    }
    return true;
}

bool bpm(vector<vector<bool>> bpGraph, int u,
         bool seen[], vector<int> matchR, int M, int N)
{
    for (int v = 0; v < N; v++)
    {
        if (bpGraph[u][v] && !seen[v])
        {
            // Mark v as visited
            seen[v] = true;

            if (matchR[v] < 0 || bpm(bpGraph, matchR[v], seen, matchR, N, M))
            {
                matchR[v] = u;
                return true;
            }
        }
    }
    return false;
}

int maxBPM(vector<vector<bool>> bpGraph, int M, int N, vector<int> project_list)
{
    vector<int> matchR(N, -1);

    int result = 0;
    for (int u = 0; u < M; u++)
    {
        bool seen[N];
        memset(seen, 0, sizeof(seen));

        if (bpm(bpGraph, u, seen, matchR, N, M) && project_list[u] != 0)
            result++;
    }
    return result;
}

vector<vector<vector<int>>> get_all_permutations(vector<vector<int>> vec)
{
    vector<vector<vector<int>>> all_permutations;

    if (vec.size() == 1)
    {
        all_permutations.push_back(vec);
        return all_permutations;
    }

    for (int i = 0; i < vec.size(); i++)
    {
        vector<vector<int>> new_vector = vec;
        new_vector.erase(new_vector.begin() + i);
        vector<vector<vector<int>>> sub_permutations = get_all_permutations(new_vector);
        for (vector<vector<int>> permutation : sub_permutations)
        {
            permutation.push_back(vec[i]);
            all_permutations.push_back(permutation);
        }
    }

    set<vector<vector<int>>> s;
    for (vector<vector<int>> permutation : all_permutations)
    {
        s.insert(permutation);
    }
    all_permutations.clear();
    for (vector<vector<int>> permutation : s)
    {
        all_permutations.push_back(permutation);
    }
    

    return all_permutations;
}

// get all subsets of a vector<vector<int>> using get_all_permutations by taking first k elements of the vector for k = 1 to vec.size() and then removing duplicates
vector<vector<vector<int>>> get_all_subsets(vector<vector<int>> vec)
{
    vector<vector<vector<int>>> all_subsets;
    vector<vector<vector<int>>> all_permutations = get_all_permutations(vec);
    for (int k = 1; k <= vec.size(); k++)
    {
        vector<vector<vector<int>>> new_vector;
        for (vector<vector<int>> permutation : all_permutations)
        {
            vector<vector<int>> sub_permutation;
            for (int i = 0; i < k; i++)
            {
                sub_permutation.push_back(permutation[i]);
            }
            new_vector.push_back(sub_permutation);
        }
        all_subsets.insert(all_subsets.end(), new_vector.begin(), new_vector.end());
    }
    return all_subsets;
}

int completedProjects(int N, int M, map<string, vector<int>> U, map<string, vector<int>> V)
{
    vector<vector<int>> vec;
    vector<vector<int>> U2;
    for (auto &u : U)
    {
        U2.push_back(u.second);
    }
    for (auto &v : V)
    {
        vec.push_back(V[v.first]);
    }

    vector<vector<vector<int>>> permutations;
    vector<int> max_matchings;

    permutations = get_all_subsets(vec);

    for (vector<vector<int>> permutation : permutations)
    {
        vector<int> project_list(5 * permutation.size(), 0);
        for (int i = 0; i < permutation.size(); i++)
        {
            for (int j = 0; j < 5; j++)
            {
                project_list[5 * i + j] = permutation[i][j];
            }
        }
        
        int len = project_list.size();
        vector<vector<bool>> bpGraph(N, vector<bool>(len, false));
        vector<bool> U_checked(N, false);
        for (int i = 0; i < len; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (U2[j][i % 5] >= project_list[i])
                {
                    bpGraph[j][i] = true;
                }
            }
        }
        int max_matching = maxBPM(bpGraph, N, M, project_list);
        if (max_matching == len / 5)
        {
            max_matchings.push_back(max_matching);
        }
    }
    return 0 ? max_matchings.size() == 0 : *max_element(max_matchings.begin(), max_matchings.end());
}

void readParticipants(map<string, vector<int>> &participants, int N)
{
    cin.ignore();
    for (int i = 0; i < N; i++)
    {
        string line;
        getline(cin, line);
        istringstream iss(line);
        string roll_number;
        vector<int> skill(5, 0);
        iss >> roll_number;
        for (int j = 0; j < 5; j++)
        {
            string text;
            iss >> text;
            if (isInteger(text))
            {
                skill[j] = stoi(text);
            }
        }
        participants[roll_number] = skill;
    }
}

void readProjects(map<string, vector<int>> &projects, int M)
{
    cin.ignore();
    for (int i = 0; i < M; i++)
    {
        string line;
        getline(cin, line);
        istringstream iss(line);
        string project_name;
        vector<int> skill(5, 0);
        iss >> project_name;
        for (int j = 0; j < 5; j++)
        {
            string text;
            iss >> text;
            if (isInteger(text))
            {
                skill[j] = stoi(text);
            }
        }
        projects[project_name] = skill;
    }
}

int main()
{
    int N;
    cin >> N;
    map<string, vector<int>> participants;
    readParticipants(participants, N);

    int M;
    cin >> M;
    map<string, vector<int>> projects;
    readProjects(projects, M);

    cout << completedProjects(N, M, participants, projects) << endl;

    return 0;
}
