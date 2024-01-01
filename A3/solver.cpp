#include <bits/stdc++.h>

std::vector<std::string>  clauses;
void size_clauses(std::string &outp, std::vector<std::string> &x, std::vector<std::vector<std::string> > &dp, int k, int n,
                  int &num_clause, std::vector<bool> &vis) {
    int c = 0;
    for (int i = 1; i <= n; i++) {
        if (!vis[i]) {
            c++;
            // clause for i,1, i>1
            if (k >= 1) {
                if (c > 1) {
                    num_clause += 3;
                    // not sim1j or sij
                    clauses.push_back( "-" + dp[c-1][1] + " " + dp[c][1] + " 0\n");
                    // not xi or sij
                    clauses.push_back( "-" + x[i] + " " + dp[c][1] + " 0\n");
                    // not sij or sim1j or xi
                    clauses.push_back("-" + dp[c][1] + " " + dp[c-1][1] + " " + x[i] + " 0\n");
                }
            }
            for (int j = 2; j < std::min(c, k + 1); j++) {
                num_clause += 4;
                // not sim1j or sij
                clauses.push_back( "-" + dp[c-1][j] + " " + dp[c][j] + " 0\n");
                // not xi or not sim1jm1 or sij
                clauses.push_back( "-" + x[i] + " -" + dp[c-1][j-1] + " " + dp[c][j] + " 0\n");
                // not sij or sim1j or sim1jm1
                clauses.push_back( "-" + dp[c][j] + " " + dp[c-1][j] + " " + dp[c-1][j-1] + " 0\n");
                // not sij or xi or sim1j
                clauses.push_back( "-" + dp[c][j] + " " + x[i] + " " + dp[c-1][j] + " 0\n");
            }
            // clause for i,i
            if (k >= c) {
                if (c == 1) {
                    num_clause += 2;
                    // not xi or sij
                    clauses.push_back( "-" + x[i] + " " + dp[c][1] + " 0\n");
                    // not sij or xi
                    clauses.push_back( "-" + dp[c][1] + " " + x[i] + " 0\n");
                } else {
                    num_clause += 3;
                    // not xi or not sim1jm1 or sij
                    clauses.push_back("-" + x[i] + " -" + dp[c-1][c-1] + " " + dp[c][c] + " 0\n");
                    // not sij or xi
                    clauses.push_back("-" + dp[c][c] + " " + x[i] + " 0\n");
                    // not sij or sim1jm1
                    clauses.push_back( "-" + dp[c][c] + " " + dp[c-1][c-1] + " 0\n");
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        //std::cout << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    std::ios_base::sync_with_stdio(false);
    std::string input_file = argv[1];
    std::ifstream ifs(input_file + ".graph");
    if (!ifs) {
        //std::cout << "Error: cannot open file " << (input_file + ".graph")
                  //<< std::endl;
        return 1;
    }
    int n, m, k1, k2;
    std::vector<std::vector<int>> adj_mat;
    ifs >> n >> m >> k1 >> k2;
    adj_mat.resize(n + 1, std::vector<int>(n + 1, 0));
    for (int i = 0; i < m; ++i) {
        int u, v;
        ifs >> u >> v;
        adj_mat[u][v] = 1;
        adj_mat[v][u] = 1;
    }
        //std::cout <<"Here"<<std::endl;

    ifs.close();
    std::queue<int> q;
    std::vector<bool> visited1(n + 1, false);
    std::vector<int> degree(n + 1, 0);
    for (int i = 1; i <= n; ++i) {
        degree[i] = std::accumulate(adj_mat[i].begin(), adj_mat[i].end(), 0);
    }
    for (int i = 1; i <= n; ++i) {
        if (degree[i] < k1-1) {
            q.push(i);
            visited1[i] = true;
        }
    }
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int v = 1; v <= n; ++v) {
            if (adj_mat[u][v] && !visited1[v]) {
                degree[v]--;
                if (degree[v] < k1-1) {
                    q.push(v);
                    visited1[v] = true;
                }
            }
        }
    }

    int pos1 = n - std::accumulate(visited1.begin(), visited1.end(), 0);
    bool fail = pos1 < k1;;
    std::vector<bool> visited2(n + 1, false);
    degree.assign(n + 1, 0);
    for (int i = 1; i <= n; ++i) {
        degree[i] = std::accumulate(adj_mat[i].begin(), adj_mat[i].end(), 0);
    }
    for (int i = 1; i <= n; ++i) {
        if (degree[i] < k2-1) {
            q.push(i);
            visited2[i] = true;
        }
    }
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int v = 1; v <= n; ++v) {
            if (adj_mat[u][v] && !visited2[v]) {
                degree[v]--;
                if (degree[v] < k2-1) {
                    q.push(v);
                    visited2[v] = true;
                }
            }
        }
    }
    int pos2 = n - std::accumulate(visited2.begin(), visited2.end(), 0);
    fail = fail || (pos2 < k2);

    std::ofstream ofs(input_file + ".satinput");
    if (fail) {
        ofs << "p cnf 0 0\n";
    } else {
        int num_var = 0;
        std::vector<std::string> x1(n + 1), x2(n + 1);
        std::vector<std::vector<std::string> > dp1(pos1 + 1, std::vector<std::string> (k1+1));
        std::vector<std::vector<std::string> > dp2(pos2 + 1, std::vector<std::string> (k2+1));
        for (int i = 1; i<=n; i++) {
            x1[i] = std::to_string(i);
        }
        for (int i = 1; i<=n; i++) {
            x2[i] = std::to_string(i+n);
        }
        num_var += 2 * n;
        for (int i = 1; i <= pos1; i++) {
            for (int j = 1; j <= std::min(i,k1); j++) {
                dp1[i][j] = std::to_string(num_var + 1);
                num_var++;
            }
        }
        for (int i = 1; i <= pos2; i++) {
            for (int j = 1; j <= std::min(i,k2); j++) {
                dp2[i][j] = std::to_string(num_var + 1);
                num_var++;
            }
        }
        int num_clause = 0;
        std::string outp = "";

        for (int i = 1; i <= n; i++) {
            if (!visited1[i] && !visited2[i]) {
                num_clause++;
                clauses.push_back("-" + x1[i] + " -" + x2[i] + " 0\n");
            }
            if (visited1[i]) {
                num_clause++;
                //not xi
                clauses.push_back("-" + x1[i] + " 0\n");
            }
            if (visited2[i]) {
                num_clause++;
                //not xi
                clauses.push_back("-" + x2[i] + " 0\n");
            }
            for (int j = i + 1; j <= n; j++) {
                if (!adj_mat[i][j]) {
                    if (!visited1[i] && !visited1[j]) {
                        num_clause++;
                        //not xi or not xj
                        clauses.push_back("-" + x1[i] + " -" + x1[j] + " 0\n");
                    }
                    if (!visited2[i] && !visited2[j]) {
                        num_clause++;
                        //not xi or not xj
                        clauses.push_back("-" + x2[i] + " -" + x2[j] + " 0\n");
                    }
                }
            }
        }
        size_clauses(outp, x1, dp1, k1, n, num_clause, visited1);
        size_clauses(outp, x2, dp2, k2, n, num_clause, visited2);
        if (k1>=1) { 
            num_clause += 1;
            //s pos1 k1
            clauses.push_back(dp1[pos1][k1] + " 0\n");
        }
        if (k2>=1) {
            num_clause += 1;
            //s pos2 k2
            clauses.push_back(dp2[pos2][k2] + " 0\n");
        }
        std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        std::shuffle(clauses.begin(),clauses.end(),rng);
        ofs << "p cnf " << num_var << " " << num_clause << "\n";
        for(auto x:clauses) {
            ofs<<x;
        }
        //std::cout <<pos1<<" "<<pos2<< std::endl;
    }
    ofs.close();
}