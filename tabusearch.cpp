#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <ctime>
#include <queue>
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
    queue<vector<bool>> tabuList; // 禁忌表，用queue實現FIFO
    int tabuListSize;             // 禁忌表大小
    vector<bool> hasselected;     // 記錄已選擇的物品

    // 檢查解是否在禁忌表中
    bool isInTabuList(const vector<bool>& solution) {
        queue<vector<bool>> tempTabuList = tabuList;
        while(!tempTabuList.empty()) {
            if(tempTabuList.front() == solution) {
                return true;
            }
            tempTabuList.pop();
        }
        return false;
    }

    // 隨機變化函數Tweak，返回變化後的解
    vector<bool> Tweak(const vector<bool>& solution) {
        vector<bool> newSolution = solution;
        int currentWeight = 0; // 追蹤當前重量
        for (int i = 0; i < N; i++) {
            if (newSolution[i]) {
                currentWeight += weight[i];
            }
        }

        bool itemAdded = false; 
        for(int i = 0; i < 5; i++) {
            int x = rand() % N;
            if (!newSolution[x] && currentWeight + weight[x] <= W) {
                newSolution[x] = true;
                hasselected[x] = true;
                currentWeight += weight[x];
                itemAdded = true;
            }
        }

        if (!itemAdded) {
            // 如果無法新增物品，則移除兩個隨機選擇的物品
            vector<int> selectedItems;
            for(int i = 0; i < N; i++) {
                if(newSolution[i]) selectedItems.push_back(i);
            }

            for(int i = 0; i < 2 && !selectedItems.empty(); i++) {
                int idx = rand() % selectedItems.size();
                int x = selectedItems[idx];
                newSolution[x] = false;
                hasselected[x] = false;
                currentWeight -= weight[x];
                selectedItems.erase(selectedItems.begin() + idx);
            }
            //並且隨機新增物品
            for(int i = 0; i < 5; i++) {
                int x = rand() % N;
                if(W - weight[x] >= 0) {
                    newSolution[x] = true;
                    hasselected[x] = true;
                    currentWeight += weight[x];
                }
            }
        }

        return newSolution;
    }

public:
    KnapsackTabu(int n, int w) : N(n), W(w) {
        profit.resize(N);
        weight.resize(N);
        currentSolution.resize(N, false);
        bestSolution.resize(N, false);
        hasselected.resize(N, false);
        bestProfit = 0;
        tabuListSize = 10;  // 設定禁忌表長度為10
    }

    void setItems(int w, int p, int index) {
        weight[index] = w;
        profit[index] = p;
    }

    // 添加解到禁忌表
    void addToTabuList(const vector<bool>& solution) {
        if (tabuList.size() >= tabuListSize) {
            tabuList.pop();  // 如果已滿，先移除最舊的解
        }
        tabuList.push(solution);  // 加入新解
    }

    void initial() {
        int currentWeight = 0; // 新增當前重量追蹤
        for (int i = 0; i < N; i++) {
            int x = rand() % N;
            if (weight[x] + currentWeight <= W && !hasselected[x]) {
                currentSolution[x] = true;
                hasselected[x] = true;
                currentWeight += weight[x];
            }
        }

        bestSolution = currentSolution;
        bestProfit = calculateProfit(currentSolution);

        addToTabuList(currentSolution);
    }


    void solve() {
        //範例第10行
        vector<bool> SR = Tweak(currentSolution);
        //範例第11行到14行
        for(int i=0;i<1000;i++){
            vector<bool> SW = Tweak(currentSolution);
            if(!isInTabuList(SW)){
                if((calculateProfit(SW)>calculateProfit(SR))||isInTabuList(SR)){
                    SR = SW;
                    //addToTabuList(currentSolution);
                }
            }
        }
        //範例第15行
        if(!isInTabuList(SR)){
            currentSolution = SR;
            addToTabuList(currentSolution);
        }
        //範例第18行
        if(calculateProfit(currentSolution)>bestProfit){
            bestSolution = currentSolution;
            bestProfit = calculateProfit(currentSolution);
        }
        //5. 輸出最佳解

    }

    //計算當前解的價值
    int calculateProfit(const vector<bool>& solution) {
        int totalWeight = 0;
        int profit = 0;
        for (int i = 0; i < N; i++) {
            if (solution[i]) {
                totalWeight += weight[i];
                profit += this->profit[i];
            }
        }

        // 若總重量超過容量，返回0（視為不合法解）
        return (totalWeight <= W) ? profit : 0;
    }


    // 獲取最佳解的值
    int getBestProfit() const {
        return bestProfit;
    }

    // 獲取最佳解
    vector<bool> getBestSolution() const {
        return bestSolution;
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

        KnapsackTabu knapsack(N, W);
        
        cout << "請輸入每個物品的重量和價值:\n";
        for (int i = 0; i < N; i++) {
            cout << "物品 " << i+1 << ": ";
            int w, p;
            cin >> w >> p;
            if (cin.fail()) {
                throw runtime_error("輸入格式錯誤");
            }
            knapsack.setItems(w, p, i);
        }
        //初始化禁忌表
        knapsack.initial();
        //執行禁忌搜索
        knapsack.solve();
        //5. 輸出最佳解
        cout << "max profit:" << knapsack.getBestProfit() << endl;
        cout << "solution:";
        vector<bool> best = knapsack.getBestSolution();
        for(int i = 0; i < N; i++) {
            cout << (best[i] ? "1" : "0");
        }
        cout << endl;

    } catch (const exception& e) {
        cerr << "錯誤: " << e.what() << endl;
        return 1;
    }
    //system("pause");
    return 0;
    
} 