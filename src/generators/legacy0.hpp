#include <bacs/problem/single/error.hpp>
#include <bacs/problem/single/generator.hpp>

#include <mysql.h>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include <mutex>

namespace bacs {
namespace problem {
namespace single {
namespace generators {

struct invalid_problem_error : virtual error {};

struct mysql_error : virtual error {
  using error = boost::error_info<struct tag_error, std::string>;
  using query = boost::error_info<struct tag_query, std::string>;
};

class db_connection {
 public:
  struct options {
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int) {
      ar & BOOST_SERIALIZATION_NVP(host);
      ar & BOOST_SERIALIZATION_NVP(port);
      ar & BOOST_SERIALIZATION_NVP(user);
      ar & BOOST_SERIALIZATION_NVP(password);
      ar & BOOST_SERIALIZATION_NVP(database);
    }

    std::string host;
    unsigned port = 0;
    std::string user;
    std::string password;
    std::string database;
  };

  explicit db_connection(const options &options_);
  ~db_connection();

  std::string escape(const std::string &s);
  void query(const std::string &q);

 public:
  static std::shared_ptr<db_connection> instance(const options &options_);

 private:
  std::mutex m_lock;
  MYSQL *m_impl;
};

class legacy0 : public generator {
 public:
  struct config {
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int) {
      ar & BOOST_SERIALIZATION_NVP(internal0);
      ar & BOOST_SERIALIZATION_NVP(db);
    }

    boost::property_tree::ptree internal0;
    db_connection::options db;
  };

 public:
  explicit legacy0(const boost::property_tree::ptree &config);

  Problem generate(const options &options_) override;

 private:
  const config m_config;
  const generator_ptr m_internal0;
  std::shared_ptr<db_connection> m_connection;
};

}  // namespace generators
}  // namespace single
}  // namespace problem
}  // namespace bacs
