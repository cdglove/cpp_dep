// *****************************************************************************
// 
// cpp_dep/inferred_include_visitor.hpp
//
// Implements logic that can generate infered includes besed on includes that
// are missing due to ifdefs. Usefule for building a full include tree.
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#pragma once
#ifndef CPPDEP_INFERREDINCLUDEVISITOR_HPP_
#define CPPDEP_INFERREDINCLUDEVISITOR_HPP_

#include "cpp_dep/cpp_dep.hpp"

// -----------------------------------------------------------------------------
//
namespace cpp_dep {

template<typename Derived>
class inferred_include_visitor
{
protected:

    void visit(include_graph_t const& g)
    {
        current_include_index_ = 0;
        current_include_count_.resize(boost::num_vertices(g), 0);
        dfs_visitor<VisitPolicy::Initial> dfs(this);
        boost::depth_first_search(g, boost::visitor(dfs));
    }
    
    int get_current_include_count(include_vertex_descriptor_t const& v) const
    {
        return current_include_count_[v];
    }

    int get_current_include_index() const
    {
        return include_index_stack_.back();
    }

private:

    void root_file(include_vertex_descriptor_t const& v, include_graph_t const& g)
    {}

    void include_file(include_vertex_descriptor_t const& v, include_graph_t const& g)
    {}

    void finish_file(include_vertex_descriptor_t const& v, include_graph_t const& g)
    {}
    
    Derived& derived()
    {
        return *static_cast<Derived*>(this);
    }

    struct terminator {};
    enum class VisitPolicy
    {
        Initial,
        Recursing,
    };

    template<VisitPolicy Policy>
    struct dfs_visitor : boost::default_dfs_visitor
    {

        dfs_visitor(inferred_include_visitor* owner)
            : owner_(owner)
        {}

        void start_vertex(include_vertex_descriptor_t const& v, include_graph_t const& g)
        {
            if(!v && is_recursing())
                throw terminator();

            if(!is_recursing())
            {
                owner_->include_index_stack_.push_back(owner_->current_include_index_++);
                owner_->derived().root_file(v, g);
            }
            else
            {
                owner_->include_index_stack_.push_back(owner_->current_include_index_++);
                owner_->derived().include_file(v, g);
            }
        }

        void examine_edge(include_edge_descriptor_t const& e, include_graph_t const& g)
        {
            // If this is the first time we encounter this vertex.
            if(owner_->current_include_count_[e.m_target]++ && !is_recursing())
            {
                try 
                {
                    dfs_visitor<VisitPolicy::Recursing> recurse(owner_);
                    boost::depth_first_search(g, boost::visitor(recurse).root_vertex(e.m_target));
                }
                catch(terminator)
                {}
            }
            else
            {
                owner_->include_index_stack_.push_back(owner_->current_include_index_++);
                owner_->derived().include_file(e.m_target, g);
            }
        }

        void finish_vertex(include_vertex_descriptor_t const& v, include_graph_t const& g)
        {
            owner_->derived().finish_file(v, g);
            owner_->include_index_stack_.pop_back();
        }

    private:

        bool is_recursing() const
        {
            return Policy == VisitPolicy::Recursing;
        }

        inferred_include_visitor* owner_;
    };

    std::vector<int> current_include_count_;
    std::vector<int> include_index_stack_;
    int current_include_index_;
};

} // namespace cpp_dep

#endif // CPPDEP_INFERREDINCLUDEVISITOR_HPP_