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
#include "ndn-repo-client/command_checker.hpp"
#include "ndn-cxx/encoding/block-helpers.hpp"

#include <chrono>
#include <thread>


BOOST_AUTO_TEST_CASE(check_insert)
{
    ndn::Face m_face;
    CommandChecker _checker(m_face);

    ndn::span<const uint8_t> request_no{};
    _checker.check_insert(ndn::Name("repo_name"),request_no,[](auto){});
    
}

BOOST_AUTO_TEST_CASE(check_delete)
{
    ndn::Face m_face;
    CommandChecker _checker(m_face);

    ndn::span<const uint8_t> request_no{};
    _checker.check_delete(ndn::Name("repo_name"),request_no,[](auto){});
    
}
