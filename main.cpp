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


class OrderBook
{
private:
    int lowestPrice;
    int highestPrice;
    int amountOfPrices;
    int poolSize;

    PriceTrack* askTracker;
    PriceTrack* bidTracker;
    Order* orderPool;
    std::queue<int> poolQueue;

    void AddAsk(Order order)
    {
        int priceIndex = order.price - lowestPrice;
        int assignedPoolIndex = poolQueue.front();
        poolQueue.pop();

        orderPool[assignedPoolIndex] = order;
    
        if(askTracker[priceIndex].tailIndex == -1)
        {
            // No asks at that price yet
            askTracker[priceIndex].headIndex = assignedPoolIndex;
            askTracker[priceIndex].tailIndex = assignedPoolIndex;
        }
        else
        {
            // Add onto existing chain
            int tailIndex = askTracker[priceIndex].tailIndex;
            orderPool[tailIndex].nextIndex = assignedPoolIndex;
            orderPool[assignedPoolIndex].lastIndex = tailIndex;
            askTracker[priceIndex].tailIndex = assignedPoolIndex;
        }

        askTracker[priceIndex].size += order.amount;
    }

    void RemoveAsk(int orderIndex)
    {
        Order cancelledOrder = orderPool[orderIndex];
        int orderSize = orderPool[orderIndex].amount;
        if(askTracker[cancelledOrder.price - lowestPrice].tailIndex == orderIndex)
        {
            // If the order is the tail
            orderPool[orderPool[orderIndex].lastIndex].nextIndex = -1;
            askTracker[cancelledOrder.price - lowestPrice].tailIndex = orderPool[orderIndex].lastIndex;
        }
        else if(askTracker[cancelledOrder.price - lowestPrice].headIndex == orderIndex)
        {
            // If the order is the head
            orderPool[orderPool[orderIndex].nextIndex].lastIndex = -1;
            askTracker[cancelledOrder.price - lowestPrice].headIndex = orderPool[orderIndex].nextIndex;
        }
        else
        {
            // If the order is in the middle
            int next = orderPool[orderIndex].nextIndex;
            int before = orderPool[orderIndex].lastIndex;
            orderPool[before].nextIndex = next;
            orderPool[next].lastIndex = before;
        }
        askTracker[cancelledOrder.price].size -= orderSize;
    }

public:
    OrderBook(int minPrice, int maxPrice, int ordersPerLevel)
    {
        lowestPrice = minPrice;
        highestPrice = maxPrice;
        
        int amountOfPrices = maxPrice - minPrice;
        poolSize =  amountOfPrices * ordersPerLevel;

        askTracker = new PriceTrack[amountOfPrices];
        orderPool = new Order[poolSize];
        
        for(int i = 0; i < poolSize; i++)
        {
            poolQueue.push(i);
        }
    }

    ~OrderBook()
    {
        delete[] askTracker;
        delete[] orderPool;
    }
};

// ADD UPDATE HEAD IN PRICETRACK LOGIC
void RemoveAsk(Order* orderPool, std::queue<int>* poolQueue, PriceTrack* priceTracker, int orderIndex, int lowestPrice)
{
    Order cancelledOrder = orderPool[orderIndex];
    int orderSize = orderPool[orderIndex].amount;
    if(priceTracker[cancelledOrder.price - lowestPrice].tailIndex == orderIndex)
    {
        // If the order is the tail
        orderPool[orderPool[orderIndex].lastIndex].nextIndex = -1;
        priceTracker[cancelledOrder.price - lowestPrice].tailIndex = orderPool[orderIndex].lastIndex;
    }
    else if(priceTracker[cancelledOrder.price - lowestPrice].headIndex == orderIndex)
    {
        // If the order is the head
        orderPool[orderPool[orderIndex].nextIndex].lastIndex = -1;
        priceTracker[cancelledOrder.price - lowestPrice].headIndex = orderPool[orderIndex].nextIndex;
    }
    else
    {
        // If the order is in the middle
        int next = orderPool[orderIndex].nextIndex;
        int before = orderPool[orderIndex].lastIndex;
        orderPool[before].nextIndex = next;
        orderPool[next].lastIndex = before;
    }

    priceTracker[cancelledOrder.price].size -= orderSize;
}

void AddAsk(Order* orderPool, std::queue<int>* poolQueue, PriceTrack* priceTracker, Order order, int price, int lowestPrice)
{
    int priceIndex = price - lowestPrice;
    int assignedPoolIndex = poolQueue->front();
    poolQueue->pop();

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

    priceTracker[priceIndex].size += order.amount;
}

int main()
{
    int lowestPrice{100};
    int highestPrice{1200};

    int amountOfPrices{highestPrice - lowestPrice};
    int poolSize{amountOfPrices * 1000};

    // A tracker that lets you access the chain of asks at a specific price, priceTracker[0] is for lowestPrice, and index 1 adds 1p.
    PriceTrack* priceTracker = new PriceTrack[amountOfPrices];

    // A pool that holds orders that are linked to one another, and a queue of the pool's free slots
    Order* orderPool = new Order[poolSize];
    std::queue<int> poolQueue;

    for(int i = 0; i < poolSize; i++)
    {
        poolQueue.push(i);
    }

    std::cout << "Hello Worrrld";

    // Cleanup
    delete[] orderPool;
    delete[] priceTracker;
    return 0;
}