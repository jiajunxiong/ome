#include "Order.h"

Order::Order(std::string id, std::string symbol, SIDE side, double price, ORDERTYPE style, long size, std::string client, long time)
   :m_id(id),
    m_symbol(symbol),
    m_side(side),
    m_price(price),
    m_style(style),
    m_size(size),
    m_client(client),
    m_time(time) {

}

Order::~Order() {

}