#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <iostream>
#include <map>
#include <unordered_map>
#include <list>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <array>
#include <random>

#include "Order.h"
#include "util.h"
#include "logger.h"

using namespace std;
using namespace boost::posix_time;
using namespace boost::interprocess;

typedef std::map<double, std::list<Order>, greater < double>> BID;
typedef std::map<double, std::list<Order>, less<double>> ASK;
std::unordered_map<std::string, std::pair<BID, ASK>> MacroOrderBook;

namespace logging = boost::log;

void init() {
    logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
}

void printMacroOrderBook() {
    std::stringstream ss;
    for (auto it = MacroOrderBook.begin(); it != MacroOrderBook.end(); ++it) {
        ss << "\n" << it->first << " order book" << std::endl;
        auto bid = it->second.first;
        ss << "    BID\n";
        for (auto it2 = bid.begin(); it2 != bid.end(); ++it2) {
            auto orderList = it2->second;
            for (auto it3 = orderList.begin(); it3 != orderList.end(); ++it3) {
                ss << "        (" << it3->getSize() << "@" << it3->getPrice() << ")\n";
            }
        }
        auto ask = it->second.second;
        ss << "    ASK\n";
        for (auto it2 = ask.begin(); it2 != ask.end(); ++it2) {
            auto orderList = it2->second;
            for (auto it3 = orderList.begin(); it3 != orderList.end(); ++it3) {
                ss << "        (" << it3->getSize() << "@" << it3->getPrice() << ")\n";
            }
        }
    }
    if (!ss.str().empty()){
        LOG_INFO(ss.str());
    }
}

void textOarchiveSender(message_queue &mq) {
    std::stringstream ss;
    srand(time(NULL));
    for (size_t i = 1; i <= 5; ++i) {
        //double price = (rand() % 9) + 1;
        //SIDE side = static_cast<SIDE>(rand() % 2);
        //long size = (rand() % 100) * 100;
        long time = abs(util::getLongTime(util::getLocalTimeInMicroSecond()));
        double price = i + 1;
        SIDE side = BUY;
        long size = 100 * i;
        Order order("123456", "0700.HK", side, price, LIMIT, size, "OME", time);

        ss << "Sending " << order << std::endl;
        std::stringstream oss;
        boost::archive::text_oarchive oa(oss);
        oa << order;
        std::string serialized_string(oss.str());
        mq.send(serialized_string.data(), serialized_string.size(), 0);
    }
    for (size_t i = 1; i <= 5; ++i) {
        //double price = (rand() % 9) + 1;
        //SIDE side = static_cast<SIDE>(rand() % 2);
        //long size = (rand() % 100) * 100;
        long time = abs(util::getLongTime(util::getLocalTimeInMicroSecond()));
        double price = i + 1;
        SIDE side = SELL;
        long size = 100 * i;
        Order order("123456", "0700.HK", side, price, LIMIT, size, "OME", time);

        ss << "Sending " << order << std::endl;
        std::stringstream oss;
        boost::archive::text_oarchive oa(oss);
        oa << order;
        std::string serialized_string(oss.str());
        mq.send(serialized_string.data(), serialized_string.size(), 0);
    }

    for (size_t i = 1; i <= 5; ++i) {
        //double price = (rand() % 9) + 1;
        //SIDE side = static_cast<SIDE>(rand() % 2);
        //long size = (rand() % 100) * 100;
        long time = abs(util::getLongTime(util::getLocalTimeInMicroSecond()));
        double price = i + 1;
        SIDE side = BUY;
        long size = 100 * i;
        Order order("123456", "0005.HK", side, price, LIMIT, size, "OME", time);

        ss << "Sending " << order << std::endl;
        std::stringstream oss;
        boost::archive::text_oarchive oa(oss);
        oa << order;
        std::string serialized_string(oss.str());
        mq.send(serialized_string.data(), serialized_string.size(), 0);
    }
    for (size_t i = 1; i <= 5; ++i) {
        //double price = (rand() % 9) + 1;
        //SIDE side = static_cast<SIDE>(rand() % 2);
        //long size = (rand() % 100) * 100;
        long time = abs(util::getLongTime(util::getLocalTimeInMicroSecond()));
        double price = i + 1;
        SIDE side = SELL;
        long size = 100 * i;
        Order order("123456", "0005.HK", side, price, LIMIT, size, "OME", time);
        ss << "Sending " << order << std::endl;

        std::stringstream oss;
        boost::archive::text_oarchive oa(oss);
        oa << order;
        std::string serialized_string(oss.str());
        mq.send(serialized_string.data(), serialized_string.size(), 0);
    }
    if (!ss.str().empty()){
        LOG_INFO(ss.str());
    }
}

void sender() {
    try {
        message_queue::remove("OME");
        message_queue mq(create_only, "OME", 100, sizeof(Order));
        textOarchiveSender(mq);
    } catch (interprocess_exception &ex) {
        LOG_ERROR("exp: " << ex.what());
        return;
    }
}

bool checkOrder(const Order &order) {
    if (order.getSize() <= 0) {
        LOG_ERROR(order << " quantity smaller than 0");
        return false;
    }

    if (order.getPrice() < 0.0) {
        LOG_ERROR(order << " price smaller than 0");
        return false;
    }

    if (order.getStyle() != MARKET && order.getStyle() != LIMIT) {
        LOG_ERROR(order << " type should be MARKET/LIMIT");
        return false;
    }

    return true;
}

void insertOrder(Order &order) {
    double price = order.getPrice();
    std::string symbol = order.getSymbol();
    SIDE side = order.getSide();
    long time = order.getTime();
    auto &orderBook = MacroOrderBook[symbol];
    auto &bid = orderBook.first;
    auto &ask = orderBook.second;
    double bestBid = bid.begin()->first;
    double bestAsk = ask.begin()->first;

    std::stringstream ss;
    if (side == BUY) {
        if (bid.find(price) != bid.end()) {
            auto &l = bid[price];
            for (auto it = l.begin(); it != l.end(); ++it) {
                if (it->getTime() > time) {
                    l.insert(it, order);
                }
                if (time >= l.back().getTime()) {
                    l.push_back(order);
                }
            }
            ss << "Find price level=" << price << ", inserting " << order.getSize() << "@" << order.getPrice()
               << std::endl;
        } else if (ask.empty() || price < bestAsk) {
            std::list<Order> l{order};
            bid.insert({price, l});
            ss << "Cannot find price level=" << price << ", creating and inserting " << order.getSize() << "@"
               << order.getPrice() << std::endl;
        }
    } else if (side == SELL) {
        if (ask.find(price) != ask.end()) {
            auto &l = ask[price];
            for (auto it = l.begin(); it != l.end(); ++it) {
                if (it->getTime() > time) {
                    l.insert(it, order);
                }
                if (time > l.back().getTime()) {
                    l.push_back(order);
                }
            }
            ss << "Find price level=" << price << ", inserting " << order.getSize() << "@" << order.getPrice()
               << std::endl;
        } else if (bid.empty() || price > bestBid) {
            std::list<Order> l{order};
            ask.insert({price, l});
            ss << "Cannot find price level=" << price << ", creating and inserting " << order.getSize() << "@"
               << order.getPrice() << std::endl;
        }
    }
    if (!ss.str().empty()){
        LOG_INFO(ss.str());
    }
}

void crossOrder(Order &order) {
    double price = order.getPrice();
    long size = order.getSize();
    std::string symbol = order.getSymbol();
    SIDE side = order.getSide();
    auto &orderBook = MacroOrderBook[symbol];
    auto &bid = orderBook.first;
    auto &ask = orderBook.second;
    double bestBid = bid.begin()->first;
    double bestAsk = ask.begin()->first;


    std::stringstream ss;
    if (side == BUY && (!ask.empty() && price >= bestAsk && size > 0)) {
        ss << "Crossing sell " << order.getSize() << "@" << order.getPrice();
        for (auto it = ask.begin(); it != ask.end(); ++it) {
            if (price >= it->first && size > 0) {
                auto &l = it->second;
                for (auto itl = l.begin(); itl != l.end(); ++itl) {
                    if (size >= itl->getSize()) {
                        size -= itl->getSize();
                        ss << " removing current " << itl->getSize() << "@" << itl->getPrice()
                           << " from list, remaining quantity=" << size;
                        itl = l.erase(itl);
                    } else {
                        itl->setSize(itl->getSize() - size);
                        size = 0;
                        ss << ", reset quantity " << itl->getSize() << "@" << itl->getPrice() << ", order all crossed!";
                    }
                }
                if (l.size() == 0) {
                    ss << ", removing price level=" << it->first;
                    it = ask.erase(it);
                }
            }
        }
    } else if (side == SELL && (!bid.empty() && price <= bestBid && size > 0)) {
        ss << "Crossing sell " << order.getSize() << "@" << order.getPrice();
        for (auto it = bid.begin(); it != bid.end();) {
            auto &l = it->second;
            if (price <= it->first && size > 0) {
                for (auto itl = l.begin(); itl != l.end();) {
                    if (size >= itl->getSize()) {
                        size -= itl->getSize();
                        ss << "removing current " << itl->getSize() << "@" << itl->getPrice()
                           << " from list, remaining quantity=" << size;
                        itl = l.erase(itl);
                    } else {
                        itl->setSize(itl->getSize() - size);
                        size = 0;
                        ss << ", reset quantity " << itl->getSize() << "@" << itl->getPrice() << ", order all crossed!";
                        ++itl;
                    }
                }
            }
            if (l.empty()) {
                ss << ", removing price level=" << it->first;
                it = bid.erase(it);
            } else {
                ++it;
            }
        }
        if (size > 0) {
            order.setSize(size);
            insertOrder(order);
        }
    }
    if (!ss.str().empty()){
        LOG_INFO(ss.str());
    }
}

boost::mutex mutex;

void onReceiveOrder(Order &order) {
    double price = order.getPrice();
    std::string symbol = order.getSymbol();
    SIDE side = order.getSide();

    std::stringstream ss;
    boost::lock_guard<boost::mutex> lock(mutex);
    if (checkOrder(order)) {
        if (MacroOrderBook.find(symbol) != MacroOrderBook.end()) {
            insertOrder(order);
            crossOrder(order);
        } else {
            BID bid;
            ASK ask;
            std::list<Order> l{order};
            if (side == BUY) {
                bid.insert({price, l});
            } else if (side == SELL) {
                ask.insert({price, l});
            }
            MacroOrderBook.insert({symbol, std::make_pair(bid, ask)});
            ss << "Received new " << order.getSize() << "@" << order.getPrice();
        }
    }
    if (!ss.str().empty()){
        LOG_INFO(ss.str());
    }
    printMacroOrderBook();
}

void textIarchiveReceiver(message_queue &mq) {
    message_queue::size_type recvd_size;
    unsigned int priority;
    Order order;
    std::stringstream iss;
    std::string serialized_string;
    serialized_string.resize(1000);
    mq.receive(&serialized_string[0], 1000, recvd_size, priority);
    iss << serialized_string;
    boost::archive::text_iarchive ia(iss);
    ia >> order;
    onReceiveOrder(order);
}

boost::asio::thread_pool g_pool(10);

void receiver() {
    try {
        message_queue mq(open_only, "OME");
        std::cout << util::getLocalTimeInMicroSecond() << std::endl;
        /*
        for (size_t i = 0; i != 10; ++i) {
            textIarchiveReceiver(mq);
        }
        */

        for (size_t i = 0; i != 20; ++i) {
            post(g_pool, boost::bind(textIarchiveReceiver, std::ref(mq)));
        }
        g_pool.join();
        std::cout << util::getLocalTimeInMicroSecond() << std::endl;

    } catch (interprocess_exception &ex) {
        LOG_ERROR("exp: " << ex.what());
        return;
    }
    message_queue::remove("OME");
}

int main() {
    Logger log;
    log.Init("OME.log", 0, 0, 1, 1);

    sender();
    receiver();

    return 0;
}
