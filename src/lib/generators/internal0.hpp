#include "bacs/single/problem/generator.hpp"

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

namespace bacs{namespace single{namespace problem{namespace generators
{
    class internal0: public generator
    {
    public:
        internal0(const boost::property_tree::ptree &config);

        api::pb::problem::Problem generate(const options &options_) override;

    private:

    private:
        static bool factory_reg_hook;
    };
}}}}
