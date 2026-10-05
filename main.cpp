#include <iostream>
#include <queue>

struct Order
{
    // True for bid, false for ask
    bool bid{false};
    int amount{-1};
    int userID{-1};
    int price{-1};

    // -1 if doesn't exist (first or last)
    int lastIndex{-1};
    int nextIndex{-1};
};

struct PriceTrack
{
    int headIndex{-1};
    int tailIndex{-1};
    int size{-1};
};

// ADD UPDATE HEAD IN PRICETRACK LOGIC
void RemoveAsk(Order* orderPool, std::queue<int>* poolQueue, PriceTrack* priceTracker, int orderIndex, int lowestPrice)
{
    Order cancelledOrder = orderPool[orderIndex];
    if(priceTracker[cancelledOrder.price - lowestPrice].tailIndex == orderIndex)
    {

    }
}

void AddAsk(Order* orderPool, std::queue<int>* poolQueue, PriceTrack* priceTracker, Order order, int price, int lowestPrice)
{
    int priceIndex = price - lowestPrice;
    int assignedPoolIndex = poolQueue->front();

    orderPool[assignedPoolIndex] = order;
    
    if(priceTracker[priceIndex].tailIndex == -1)
    {
        // No asks at that price yet
        priceTracker[priceIndex].headIndex = assignedPoolIndex;
        priceTracker[priceIndex].tailIndex = assignedPoolIndex;
    }
    else
    {
        // Add onto existing chain
        int tailIndex = priceTracker[priceIndex].tailIndex;
        orderPool[tailIndex].nextIndex = assignedPoolIndex;
        orderPool[assignedPoolIndex].lastIndex = tailIndex;
        priceTracker[priceIndex].tailIndex = assignedPoolIndex;
    }
}

int main()
{
    int lowestPrice{100};
    int highestPrice{1200};
    int poolSize{highestPrice - lowestPrice};

    // A tracker that lets you access the chain of asks at a specific price, priceTracker[0] is for lowestPrice, and index 1 adds 1p.
    PriceTrack* priceTracker = new PriceTrack[poolSize];

    // A pool that holds orders that are linked to one another, and a queue of the pool's free slots
    Order* orderPool = new Order[poolSize * 1000];
    std::queue<int> poolQueue;

    for(int i = 0; i < poolSize * 1000; i++)
    {
        poolQueue.push(i);
    }

    std::cout << "Hello World";

    // Cleanup
    delete[] orderPool;
    delete[] priceTracker;
    return 0;
}