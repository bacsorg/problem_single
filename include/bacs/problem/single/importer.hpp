#pragma once

#include <bacs/problem/single/generator.hpp>

#include <bacs/problem/importer.hpp>

namespace bacs {
namespace problem {
namespace single {

class importer : public bacs::problem::importer {
 public:
  explicit importer(const boost::property_tree::ptree &config);

  Problem convert(const options &options_) override;

 private:
  const generator_ptr m_generator;
};

}  // namespace single
}  // namespace problem
}  // namespace bacs
