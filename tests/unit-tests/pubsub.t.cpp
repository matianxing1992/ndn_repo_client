/* -*- Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2012-2021 University of California, Los Angeles
 *
 * This file is part of ndn-svs, synchronization library for distributed realtime
 * applications for NDN.
 *
 * ndn-svs library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, in version 2.1 of the License.
 *
 * ndn-svs library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 */



#include "../boost-test.hpp"
#include "ndn-repo-client/pubsub.hpp"
#include "ndn-cxx/encoding/block-helpers.hpp"

#include <chrono>
#include <thread>


struct pubsub_fixture
{
    
};

BOOST_FIXTURE_TEST_SUITE(test_pubsub, pubsub_fixture);

BOOST_AUTO_TEST_CASE(publish)
{


}

BOOST_AUTO_TEST_SUITE_END();
