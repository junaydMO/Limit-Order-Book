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

// Stores information about the order to confirm an order or a cancellation has taken place
struct OrderReceipt
{
    bool complete{false};
    bool bid{false};
    int amount{-1};
    int price{-1};
    int orderIndex{-1};
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

        // Puts the order in the slot and updates the queue
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
        int priceIndex = cancelledOrder.price - lowestPrice;
        
        if(askTracker[priceIndex].headIndex == askTracker[priceIndex].headIndex == orderIndex)
        {
            // If the only entry
        }
        else if(askTracker[priceIndex].headIndex == orderIndex)
        {
            // If head
        }
        else if(askTracker[priceIndex].tailIndex = orderIndex)
        {
            // If tail
        }
        else
        {
            // If in the middle
        }
        
        askTracker[priceIndex].size -= orderSize;
        poolQueue.push(orderIndex);
    }

public:
    // CONSTRUCTOOOORRR!!
    OrderBook(int minPrice, int maxPrice, int ordersPerLevel)
    {
        lowestPrice = minPrice;
        highestPrice = maxPrice;
        
        int amountOfPrices = maxPrice - minPrice + 1;
        poolSize =  amountOfPrices * ordersPerLevel;

        askTracker = new PriceTrack[amountOfPrices];
        orderPool = new Order[poolSize];
        
        for(int i = 0; i < poolSize; i++)
        {
            poolQueue.push(i);
        }
    }

    // Remember to use this to delete anything like that
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
        else
        {
            // Tut tut tut someone sent an invalid order!
        }
    }
};

int main()
{
    OrderBook orderBook(0, 1200, 1000);
    return 0;
}