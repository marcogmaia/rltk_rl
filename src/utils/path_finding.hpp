#include <memory>
#include "utils/astar.hpp"

namespace radl {


template <typename T, int X>
concept CNavigable = requires(T nav) {
    { nav.get_x(X) }
    ->std::integral;
    { nav.get_y(X) }
    ->std::integral;
};

template <typename location_t, typename navigator_t>
class map_search_node {
public:
    location_t pos;

    map_search_node() {}
    map_search_node(location_t loc)
        : pos(loc) {}

    float GoalDistanceEstimate(map_search_node<location_t, navigator_t>& goal) {
        float result = navigator_t::get_distance_estimate(pos, goal.pos);
        // std::cout << "GoalDistanceEstimate called (" << result << ").\n";
        return result;
    }

    bool IsGoal(map_search_node<location_t, navigator_t>& node_goal) {
        bool result = navigator_t::is_goal(pos, node_goal.pos);
        // std::cout << "IsGoal called (" << result << ").\n";
        return result;
    }

    bool GetSuccessors(
        AStarSearch<map_search_node<location_t, navigator_t>>* a_star_search,
        map_search_node<location_t, navigator_t>* parent_node) {
        // std::cout << "GetSuccessors called.\n";
        std::vector<location_t> successors;
        if(parent_node) {
            navigator_t::get_successors(parent_node->pos, successors);
        }
        else {
            navigator_t::get_successors({-1, -1}, successors);
            // throw std::runtime_error("Null parent error.");
            // return false;
        }
        for(location_t loc : successors) {
            map_search_node<location_t, navigator_t> tmp(loc);
            // std::cout << " --> " << loc.first << "/" << loc.second << "\n";
            a_star_search->AddSuccessor(tmp);
        }
        return true;
    }

    float GetCost(map_search_node<location_t, navigator_t>& successor) {
        float result = navigator_t::get_cost(pos, successor.pos);
        // std::cout << "GetCost called (" << result << ").\n";
        return result;
    }

    bool IsSameState(map_search_node<location_t, navigator_t>& rhs) {
        bool result = navigator_t::is_same_state(pos, rhs.pos);
        // std::cout << "IsSameState called (" << result << ").\n";
        return result;
    }
};


// Template class used to define what a navigation path looks like
template <class location_t>
struct navigation_path {
    bool success = false;
    location_t destination;
    std::deque<location_t> steps;
};

template <typename location_t, typename navigator_t>
std::shared_ptr<navigation_path<location_t>>
find_path_2d(const location_t& start, const location_t& end) {
    {
        auto result = std::make_shared<navigation_path<location_t>>(true, end);
        auto x1     = navigator_t::get_x(start);
        auto y1     = navigator_t::get_y(start);
        auto x2     = navigator_t::get_x(end);
        auto y2     = navigator_t::get_y(end);
        auto dx     = x2 - x1;
        auto dy     = y2 - y1;
        auto ix     = (dx == 0) ? 0 : ((dx < 0) ? -1 : 1);
        auto iy     = (dy == 0) ? 0 : ((dy < 0) ? -1 : 1);
        x1 += ix;
        y1 += iy;
        line_func(x1, y1, x2, y2, [&result](int X, int Y) {
            location_t step = navigator_t::get_xy(X, Y);
            if(result->success && navigator_t::is_walkable(step)) {
                result->steps.push_back(step);
            }
            else {
                result->success = false;
                return;
            }
        });
        if(result->success) {
            return result;
        }
    }

    AStarSearch<map_search_node<location_t, navigator_t>> a_star_search;
    map_search_node<location_t, navigator_t> a_start(start);
    map_search_node<location_t, navigator_t> a_end(end);

    a_star_search.SetStartAndGoalStates(a_start, a_end);
    unsigned int search_state;
    unsigned int search_steps = 0;

    do {
        search_state = a_star_search.SearchStep();
        ++search_steps;
    } while(
        search_state
        == AStarSearch<
            map_search_node<navigator_t, location_t>>::SEARCH_STATE_SEARCHING);

    if(search_state
       == AStarSearch<
           map_search_node<navigator_t, location_t>>::SEARCH_STATE_SUCCEEDED) {
        auto result = std::make_shared<navigation_path<location_t>>(false, end);
        map_search_node<location_t, navigator_t>* node
            = a_star_search.GetSolutionStart();
        for(;;) {
            node = a_star_search.GetSolutionNext();
            if(!node)
                break;
            result->steps.push_back(node->pos);
        }
        a_star_search.FreeSolutionNodes();
        a_star_search.EnsureMemoryFreed();
        result->success = true;
        return result;
    }

    auto result = std::make_shared<navigation_path<location_t>>(false, end);
    a_star_search.EnsureMemoryFreed();
    return result;
}


/*
 * Implements a simple A-Star path, with no line-search optimization. This has
 * the benefit of avoiding requiring as much additional translation between the
 * template and your preferred map format, at the expense of being potentially
 * slower for some paths.
 */
template <class location_t, class navigator_t>
std::shared_ptr<navigation_path<location_t>> find_path(const location_t start,
                                                       const location_t end) {
    AStarSearch<map_search_node<location_t, navigator_t>> a_star_search;
    map_search_node<location_t, navigator_t> a_start(start);
    map_search_node<location_t, navigator_t> a_end(end);

    a_star_search.SetStartAndGoalStates(a_start, a_end);
    unsigned int search_state;
    std::size_t search_steps = 0;

    do {
        search_state = a_star_search.SearchStep();
        ++search_steps;
    } while(
        search_state
        == AStarSearch<
            map_search_node<navigator_t, location_t>>::SEARCH_STATE_SEARCHING);

    if(search_state
       == AStarSearch<
           map_search_node<navigator_t, location_t>>::SEARCH_STATE_SUCCEEDED) {
        auto result         = std::make_shared<navigation_path<location_t>>();
        result->destination = end;
        map_search_node<location_t, navigator_t>* node
            = a_star_search.GetSolutionStart();
        for(;;) {
            node = a_star_search.GetSolutionNext();
            if(!node)
                break;
            result->steps.push_back(node->pos);
        }
        a_star_search.FreeSolutionNodes();
        a_star_search.EnsureMemoryFreed();
        result->success = true;
        return result;
    }

    auto result = std::make_shared<navigation_path<location_t>>(false, end);

    a_star_search.EnsureMemoryFreed();
    return result;
}


}  // namespace radl
