#include <iostream>
#include <queue>

/// || Holds the order book logic for adding and filling orders, accounts & authentication must be handled by an API that is not here.
/// || The API will handle encrypting the data to send over so that it cannot be used maliciously, so its okay that that's not done here.

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

// Stores information about the order to confirm an order or a cancellation has taken place, and to give the information they need to cancel an order (will be encrypted by the API, not here)
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

    int highestBid;
    int lowestAsk;

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

        if(priceIndex < lowestAsk)
        {
            lowestAsk = priceIndex;
        }

        FillOrders();
    }

    void RemoveAsk(int orderIndex)
    {
        Order cancelledOrder = orderPool[orderIndex];
        int orderSize = orderPool[orderIndex].amount;
        int priceIndex = cancelledOrder.price - lowestPrice;
        
        if((orderIndex == askTracker[priceIndex].headIndex) &&  (askTracker[priceIndex].headIndex == orderIndex))
        {
            // If the only entry
            askTracker[priceIndex].headIndex = -1;
            askTracker[priceIndex].tailIndex = -1;
        }
        else if(askTracker[priceIndex].headIndex == orderIndex)
        {
            // If head
            askTracker[priceIndex].headIndex = cancelledOrder.nextIndex;
            orderPool[cancelledOrder.nextIndex].lastIndex = -1;
        }
        else if(askTracker[priceIndex].tailIndex == orderIndex)
        {
            // If tail
            askTracker[priceIndex].tailIndex = cancelledOrder.lastIndex;
            orderPool[cancelledOrder.lastIndex].nextIndex = -1;
        }
        else
        {
            // If in the middle
            orderPool[cancelledOrder.lastIndex].nextIndex = cancelledOrder.nextIndex;
            orderPool[cancelledOrder.nextIndex].lastIndex = cancelledOrder.lastIndex;
        }
        
        askTracker[priceIndex].size -= orderSize;
        poolQueue.push(orderIndex);

        if(priceIndex == lowestAsk && askTracker[priceIndex].size == 0)
        {
            int temp = priceIndex;
            // Move down highest bid to the next highest
            while(bidTracker[temp].size == 0 && temp != poolSize - 1)
            {
                temp--;
            }

            if(temp == 0)
            {
                highestBid = -1;
            }
            else
            {
                highestBid = temp;
            }
        }
    }

    void AddBid(Order order)
    {
        int priceIndex = order.price - lowestPrice;

        // Puts the order in the slot and updates the queue
        int assignedPoolIndex = poolQueue.front();
        poolQueue.pop();
        orderPool[assignedPoolIndex] = order;
    
        if(bidTracker[priceIndex].tailIndex == -1)
        {
            // No asks at that price yet
            bidTracker[priceIndex].headIndex = assignedPoolIndex;
            bidTracker[priceIndex].tailIndex = assignedPoolIndex;
        }
        else
        {
            // Add onto existing chain
            int tailIndex = bidTracker[priceIndex].tailIndex;
            orderPool[tailIndex].nextIndex = assignedPoolIndex;
            orderPool[assignedPoolIndex].lastIndex = tailIndex;
            bidTracker[priceIndex].tailIndex = assignedPoolIndex;
        }

        bidTracker[priceIndex].size += order.amount;

        if(priceIndex > highestBid)
        {
            highestBid = priceIndex;
        }

        FillOrders();
    }

    void RemoveBid(int orderIndex)
    {
        Order cancelledOrder = orderPool[orderIndex];
        int orderSize = orderPool[orderIndex].amount;
        int priceIndex = cancelledOrder.price - lowestPrice;
        
        if((orderIndex == bidTracker[priceIndex].headIndex) && (bidTracker[priceIndex].headIndex == orderIndex))
        {
            // If the only entry
            bidTracker[priceIndex].headIndex = -1;
            bidTracker[priceIndex].tailIndex = -1;
        }
        else if(bidTracker[priceIndex].headIndex == orderIndex)
        {
            // If head
            bidTracker[priceIndex].headIndex = cancelledOrder.nextIndex;
            orderPool[cancelledOrder.nextIndex].lastIndex = -1;
        }
        else if(bidTracker[priceIndex].tailIndex == orderIndex)
        {
            // If tail
            bidTracker[priceIndex].tailIndex = cancelledOrder.lastIndex;
            orderPool[cancelledOrder.lastIndex].nextIndex = -1;
        }
        else
        {
            // If in the middle
            orderPool[cancelledOrder.lastIndex].nextIndex = cancelledOrder.nextIndex;
            orderPool[cancelledOrder.nextIndex].lastIndex = cancelledOrder.lastIndex;
        }
        
        bidTracker[priceIndex].size -= orderSize;
        poolQueue.push(orderIndex);

        if(priceIndex == highestBid && bidTracker[priceIndex].size == 0)
        {
            int temp = priceIndex;
            // Move down highest bid to the next highest
            while(bidTracker[temp].size == 0 && temp != 0)
            {
                temp--;
            }

            if(temp == 0)
            {
                highestBid = -1;
            }
            else
            {
                highestBid = temp;
            }
        }
    }

    void FillOrders()
    {

    }

public:
    // CONSTRUCTOOOORRR!!
    OrderBook(int minPrice, int maxPrice, int ordersPerLevel)
    {
        lowestPrice = minPrice;
        highestPrice = maxPrice;
        
        amountOfPrices = maxPrice - minPrice + 1;
        poolSize =  amountOfPrices * ordersPerLevel;

        askTracker = new PriceTrack[amountOfPrices];
        bidTracker = new PriceTrack[amountOfPrices];
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
        delete[] bidTracker;
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