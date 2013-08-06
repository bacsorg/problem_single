#include <bacs/problem/single/generator.hpp>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

namespace bacs{namespace problem{namespace single{namespace generators
{
    class internal0: public generator
    {
    public:
        explicit internal0(const boost::property_tree::ptree &config);

        problem::Problem generate(const options &options_) override;

    private:

    private:
        static const bool factory_reg_hook;
    };
}}}}
