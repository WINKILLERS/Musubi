#include "AApch.h"
#include "ActivationLevel.h"
#include "Keys.h"

#ifndef _DEBUG
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#endif // !_DEBUG

int main() {
  spdlog::set_default_logger(
      spdlog::daily_logger_mt("MusubiActivator", "Musubi.log"));
  spdlog::flush_every(3s);

  auto router = std::make_unique<restinio::router::express_router_t<>>();
  router->http_get("/generate", [](auto req, auto params) {
    const auto qp = restinio::parse_query(req->header().query());

    std::string user;
    uint32_t days;
    uint32_t level;

    try {
      user = opt_value<std::string>(qp, "user").value();
      days = opt_value<uint32_t>(qp, "days").value();
      level = opt_value<uint32_t>(qp, "level").value();

      if (level > (uint32_t)ActivationLevel::Premium)
        throw std::exception("level too large");
    } catch (const std::exception &) {
      spdlog::error("invalid parameters for {}",
                    req->remote_endpoint().address().to_string());

      return req->create_response(restinio::status_bad_request())
          .set_body("parameters invalid")
          .done();
    }

    auto token =
        jwt::create()
            .set_issuer("auth0")
            .set_type("JWT")
            .set_issued_at(std::chrono::system_clock::now())
            .set_expires_at(std::chrono::system_clock::now() +
                            std::chrono::days(days))
            .set_payload_claim("user", jwt::claim(user))
            .set_payload_claim("level", jwt::claim(std::to_string(level)))
            .sign(jwt::algorithm::ed448(pub_key, priv_key, "", ""));

    spdlog::info("token {} created for {}", token,
                 req->remote_endpoint().address().to_string());

    return req->create_response().set_body(token).done();
  });

  router->http_get("/online", [](auto req, auto params) {
    const auto qp = restinio::parse_query(req->header().query());

    std::string token;
    std::string version;

    try {
      token = opt_value<std::string>(qp, "token").value();
      version = opt_value<std::string>(qp, "version").value();
    } catch (const std::exception &) {
      spdlog::error("invalid parameters for {}",
                    req->remote_endpoint().address().to_string());

      return req->create_response(restinio::status_bad_request())
          .set_body("parameters invalid")
          .done();
    }

    std::hash<std::string> hasher;
    std::vector<size_t> blacklist;
    nlohmann::json blacklist_json;
    std::ifstream blacklist_file("blacklist.json");
    if (blacklist_file.is_open() == false) {
      return req->create_response(restinio::status_bad_request())
          .set_body("can not open file")
          .done();
    }

    try {
      blacklist_json = nlohmann::json::parse(blacklist_file);

      for (const auto &node : blacklist_json) {
        auto token_hash = hasher(node["token"]);
        blacklist.push_back(token_hash);
      }
    } catch (const std::exception &) {
    }
    blacklist_file.close();

    try {
      auto decoded = jwt::decode(token);
      auto verifier =
          jwt::verify()
              .allow_algorithm(jwt::algorithm::ed448(pub_key, "", "", ""))
              .with_issuer("auth0");

      verifier.verify(decoded);

      auto expire_days =
          std::chrono::duration_cast<std::chrono::days>(
              decoded.get_expires_at() - std::chrono::system_clock::now())
              .count();
      auto user = decoded.get_payload_claim("user").as_string();
      auto level = (ActivationLevel)std::stoul(
          decoded.get_payload_claim("level").as_string());

      spdlog::info("version: {}, token {}, user: {}, expire_days:{}, level: "
                   "{}, online with ip: {}",
                   version, token, user, expire_days,
                   magic_enum::enum_name(level),
                   req->remote_endpoint().address().to_string());

      auto hash = hasher(token);
      auto find_result = std::find(blacklist.cbegin(), blacklist.cend(), hash);
      if (find_result != blacklist.cend()) {
        throw std::exception();
      }

      return req->create_response().set_body("verification passed").done();
    } catch (const std::exception &) {
      spdlog::error("verification not passed");

      return req->create_response(restinio::status_forbidden())
          .set_body("verification not passed")
          .done();
    }
  });

  router->http_get("/blacklist/add", [](auto req, auto params) {
    const auto qp = restinio::parse_query(req->header().query());

    std::string token;

    try {
      token = opt_value<std::string>(qp, "token").value();
    } catch (const std::exception &) {
      spdlog::error("invalid parameters for {}",
                    req->remote_endpoint().address().to_string());

      return req->create_response(restinio::status_bad_request())
          .set_body("parameters invalid")
          .done();
    }

    nlohmann::json blacklist_json;
    std::fstream blacklist_file("blacklist.json", std::ios::in | std::ios::out);
    if (blacklist_file.is_open() == false) {
      return req->create_response(restinio::status_bad_request())
          .set_body("can not open file")
          .done();
    }

    try {
      blacklist_json = nlohmann::json::parse(blacklist_file);
    } catch (const std::exception &) {
    }

    nlohmann::json node;
    node["token"] = token;
    node["add_time"] =
        std::chrono::system_clock::now().time_since_epoch().count();
    blacklist_json.push_back(node);
    auto json = blacklist_json.dump();

    blacklist_file.clear();
    blacklist_file.seekp(0, std::ios::beg);
    blacklist_file << json;
    blacklist_file.close();

    return req->create_response().set_body(json).done();
  });

  router->non_matched_request_handler([](auto req) {
    return req->create_response(restinio::status_not_found())
        .connection_close()
        .done();
  });

  struct auth_server_traits : public restinio::default_single_thread_traits_t {
    using request_handler_t = restinio::router::express_router_t<>;
  };

  restinio::run(restinio::on_this_thread<auth_server_traits>()
                    .address("0.0.0.0")
                    .port(11459)
                    .request_handler(std::move(router)));

  // std::cout << "input user: " << std::flush;
  // std::cin >> user;
  // std::cout << "input days: " << std::flush;
  // std::cin >> days;
  // std::cout << "input level: " << std::flush;
  // std::cin >> level;

  // auto token = jwt::create()
  //	.set_issuer("auth0")
  //	.set_type("JWT")
  //	.set_issued_at(std::chrono::system_clock::now())
  //	.set_expires_at(std::chrono::system_clock::now() +
  // std::chrono::days(days)) 	.set_payload_claim("user", jwt::claim(user))
  //	.set_payload_claim("level", jwt::claim(std::to_string(level)))
  //	.sign(jwt::algorithm::es256k(pub_key, priv_key, "", ""));

  // auto verifier = jwt::verify()
  //	.allow_algorithm(jwt::algorithm::es256k(pub_key, "", "", ""))
  //	.with_issuer("auth0");

  // try
  //{
  //	auto decoded = jwt::decode(token);

  //	spdlog::info("token expires at {} day(s)",
  // std::chrono::duration_cast<std::chrono::days>(decoded.get_expires_at() -
  // std::chrono::system_clock::now()).count());

  //	auto user = decoded.get_payload_claim("user").as_string();
  //	spdlog::info("user: {}", user);
  //	spdlog::info("token: {}", token);

  //	return 0;

  //	verifier.verify(decoded);
  //}
  // catch (const jwt::token_verification_exception&)
  //{
  //	spdlog::error("verification error, aborting");
  //	return 0;
  //}
  // catch (const std::exception&)
  //{
  //	spdlog::error("decode error, aborting");
  //	return 0;
  //}
}