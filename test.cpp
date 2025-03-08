#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <ctime>
using namespace std;

class KnapsackTabu {
private:
    int N;  // 物品數量
    int W;  // 背包容量
    vector<int> profit;  // 物品價值
    vector<int> weight;  // 物品重量
    vector<bool> currentSolution;  // 當前解
    vector<bool> bestSolution;     // 最佳解
    int bestProfit;               // 最佳價值
    vector<vector<bool>> tabuList; // 禁忌表
    int tabuListSize;             // 禁忌表大小

    // 計算當前解的總價值
    int calculateProfit(const vector<bool>& solution) {
        int totalProfit = 0;
        int totalWeight = 0;
        for (int i = 0; i < N; i++) {
            if (solution[i]) {
                totalProfit += profit[i];
                totalWeight += weight[i];
            }
        }
        return (totalWeight <= W) ? totalProfit : 0;
    }

    // 生成鄰域解
    vector<vector<bool>> generateNeighbors(const vector<bool>& solution) {
        vector<vector<bool>> neighbors;
        for (int i = 0; i < N; i++) {
            vector<bool> neighbor = solution;
            neighbor[i] = !neighbor[i];
            
            // 檢查新解是否超過背包容量
            int totalWeight = 0;
            for (int j = 0; j < N; j++) {
                if (neighbor[j]) {
                    totalWeight += weight[j];
                }
            }
            
            // 只有當總重量不超過背包容量時才加入鄰域解
            if (totalWeight <= W) {
                neighbors.push_back(neighbor);
            }
        }
        return neighbors;
    }

    // 檢查解是否在禁忌表中
    bool isTabu(const vector<bool>& solution) {
        for (const auto& tabuSolution : tabuList) {
            if (solution == tabuSolution) return true;
        }
        return false;
    }

public:
    KnapsackTabu(int n, int w, const vector<int>& p, const vector<int>& wt) 
        : N(n), W(w), profit(p), weight(wt), tabuListSize(10) {
        if (N <= 0 || W <= 0) {
            throw invalid_argument("物品數量和背包容量必須大於0");
        }
        if (profit.size() != N || weight.size() != N) {
            throw invalid_argument("物品數量和輸入數據不匹配");
        }
        
        // 初始化隨機解
        try {
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<> dis(0, 1);
            
            currentSolution.resize(N);
            bestSolution.resize(N);
            for (int i = 0; i < N; i++) {
                currentSolution[i] = dis(gen);
            }
            bestSolution = currentSolution;
            bestProfit = calculateProfit(currentSolution);
        } catch (const exception& e) {
            cerr << "初始化錯誤: " << e.what() << endl;
            throw;
        }
    }

    void solve(int maxIterations) {
        if (maxIterations <= 0) {
            throw invalid_argument("迭代次數必須大於0");
        }

        for (int iteration = 0; iteration < maxIterations; iteration++) {
            vector<vector<bool>> neighbors = generateNeighbors(currentSolution);
            vector<bool> bestNeighbor;
            int bestNeighborProfit = -1;

            // 尋找最佳鄰域解
            for (const auto& neighbor : neighbors) {
                if (!isTabu(neighbor)) {
                    int neighborProfit = calculateProfit(neighbor);
                    if (neighborProfit > bestNeighborProfit) {
                        bestNeighborProfit = neighborProfit;
                        bestNeighbor = neighbor;
                    }
                }
            }

            // 如果沒有找到更好的鄰域解，隨機選擇一個非禁忌解
            if (bestNeighbor.empty()) {
                cout<<"隨機選擇非禁忌解";
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<> dis(0, N-1);
                bestNeighbor = currentSolution;
                bestNeighbor[dis(gen)] = !bestNeighbor[dis(gen)];
            }

            // 更新解
            if (bestNeighborProfit > bestProfit) {
                bestSolution = bestNeighbor;
                bestProfit = bestNeighborProfit;
            }

            // 更新禁忌表
            tabuList.push_back(currentSolution);
            if (tabuList.size() > tabuListSize) {
                tabuList.erase(tabuList.begin());
            }

            currentSolution = bestNeighbor;
        }
    }

    void printSolution() {
        cout << "最佳解: ";
        for (bool item : bestSolution) {
            cout << item << " ";
        }
        cout << "\n最佳價值: " << bestProfit << endl;
        
        // 輸出選中物品的詳細信息
        cout << "選中的物品:\n";
        int totalWeight = 0;
        for (int i = 0; i < N; i++) {
            if (bestSolution[i]) {
                cout << "物品 " << i+1 << ": 重量=" << weight[i] 
                     << ", 價值=" << profit[i] << endl;
                totalWeight += weight[i];
            }
        }
        cout << "總重量: " << totalWeight << endl;
    }
};

int main() {
    try {
        int N, W;
        cout << "請輸入物品數量(N)和背包容量(W): ";
        cin >> N >> W;

        if (cin.fail()) {
            throw runtime_error("輸入格式錯誤");
        }

        vector<int> profit(N);
        vector<int> weight(N);

        cout << "請輸入每個物品的重量和價值:\n";
        for (int i = 0; i < N; i++) {
            cout << "物品 " << i+1 << ": ";
            cin >> weight[i] >> profit[i];
            if (cin.fail()) {
                throw runtime_error("輸入格式錯誤");
            }
        }

        KnapsackTabu knapsack(N, W, profit, weight);
        knapsack.solve(1000);  // 執行1000次迭代
        knapsack.printSolution();

    } catch (const exception& e) {
        cerr << "錯誤: " << e.what() << endl;
        return 1;
    }
    //system("pause");
    return 0;
} 