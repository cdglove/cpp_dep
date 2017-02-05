// *****************************************************************************
// 
// cpp_dep.hpp
//
// Represents the include file structure of a cpp source file.
//
// Copyright Chris Glover 2015
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#ifndef CPPDEP_CPPDEP_HPP_
#define CPPDEP_CPPDEP_HPP_

#include <string>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <iosfwd>

// -----------------------------------------------------------------------------
//
namespace cpp_dep {

// -----------------------------------------------------------------------------
//
struct include_vertex_t
{
    include_vertex_t()
        : name("")
        , size(0)
        , size_dependencies(0)
        , included_count(0)
    {}

    include_vertex_t(std::string name_, std::size_t size_)
        : name(name_)
        , size(size_)
        , size_dependencies(0)
        , included_count(0)
    {}

    std::string name;
    std::size_t size;
    std::size_t size_dependencies;
    int included_count;
};

struct include_edge_t
{

};

// Effectively a strong typedef so client
// don't need to include the header just for the
// typename.
class include_graph_t : public
    boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::directedS,
        include_vertex_t,
        include_edge_t
    > {};

typedef boost::graph_traits<
    include_graph_t
>::vertex_descriptor include_vertex_descriptor_t;

typedef boost::graph_traits<
    include_graph_t
>::edge_descriptor include_edge_descriptor_t;

// -----------------------------------------------------------------------------
// Builds the include deps graph from ain input file. The file must be in the 
// format of either gcc as output from the command;
//   g++ -H -E -o /dev/null source.cpp 2> includes.txt
// or from msvc as output from the command;
//   cl.exe /showIncludes /P source.cpp 1> nul 2> includes.txt
include_graph_t read_deps_file(char const* file);

// -----------------------------------------------------------------------------
// Inverts the include graph such that it decends by file system
// component. Ie: the root becomes to most common path, ie
//   /home/user/src/project
// followed by the next, etc.
include_graph_t invert_to_paths(include_graph_t const& g);

// -----------------------------------------------------------------------------
//
void write_graphviz(std::ostream& out, include_graph_t const& g);
}

#endif // CPPDEP_CPPDEP_HPP_
