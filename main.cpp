#include <iostream>
#include <queue>

struct Order
{
    // Cancel flag and cancel data
    bool cancel{false};
    int poolIndex;

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
    // CONSTRUCTOOOORRR!!
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

    // Remember to delete anything like this
    ~OrderBook()
    {
        delete[] askTracker;
        delete[] orderPool;
    }

    void AddOrder(Order incomingOrder)
    {
        if(incomingOrder.bid)
        {
            // Add Bid
        }
        else
        {
            // Add Ask
            AddAsk(incomingOrder);
        }
    }

    void CancelOrder(int cancelledIndex, int userID)
    {
        if(orderPool[cancelledIndex].userID == userID)
        {
            if(orderPool[cancelledIndex].bid)
            {
                // Cancel Bid
            }
            else
            {
                // Cancel Ask
                RemoveAsk(cancelledIndex);
            }
        }
    }
};

int main()
{
    OrderBook orderBook(0, 1200, 1000);
    return 0;
}