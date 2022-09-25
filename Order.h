//
// Created by Jiajun on 9/3/2022.
//

#pragma once

#ifndef OME_ORDER_H
#define OME_ORDER_H

#include <string>
#include <iostream>
#include <boost/serialization/string.hpp>
#include <boost/serialization/access.hpp>

#define MAX_SIZE 1000

enum ORDERTYPE {
    MARKET, LIMIT
};

enum SIDE {
    BUY, SELL
};

class Order {
public:
    Order() = default;

    Order(std::string id, std::string symbol, SIDE side, double price, ORDERTYPE style, long size, std::string client, long time);

    ~Order();

    std::string getId() const { return m_id; }

    std::string getSymbol() const { return m_symbol; }

    SIDE getSide() const { return m_side; }

    double getPrice() const { return m_price; }

    ORDERTYPE getStyle() const { return m_style; }

    void setSize(long size) { m_size = size; }
    long getSize() const { return m_size; }

    std::string getClient() const { return m_client; }

    long getTime() const {return m_time;}

    friend std::ostream &operator<<(std::ostream &os, const Order &order) {
        os << "Order:["
           << order.m_id << ", "
           << order.m_symbol << ", "
           << order.m_side << ", "
           << order.m_price << ", "
           << order.m_style << ", "
           << order.m_size << ", "
           << order.m_client << ", "
           << order.m_time
           << "]";
        return os;
    }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & m_id & m_symbol & m_side & m_price & m_style & m_size & m_client & m_time;
    }

    std::string m_id;
    std::string m_symbol;
    SIDE m_side;
    double m_price;
    ORDERTYPE m_style;
    long m_size;
    std::string m_client;
    long m_time;
};

#endif