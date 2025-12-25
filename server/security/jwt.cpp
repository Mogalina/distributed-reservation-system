#include "jwt.hpp"

namespace security {

std::string Security::generateToken(const std::string& userId, 
                                    const int expireMinutes) {
  auto now = std::chrono::system_clock::now();
  return jwt::create()
    .set_issuer("server")
    .set_type("JWS")
    .set_subject(userId)
    .set_issued_at(now)
    .set_expires_at(now + std::chrono::minutes{expireMinutes})
    .sign(jwt::algorithm::hs256{secret_});
}

std::optional<std::string> Security::verifyToken(const std::string& token) {
  try {
    auto decoded = jwt::decode(token);
    auto verifier = jwt::verify()
      .allow_algorithm(jwt::algorithm::hs256{secret_})
      .with_issuer("server");
    verifier.verify(decoded);
    return decoded.get_subject();

  } catch (...) {
    return std::nullopt;
  }
}

}  // namespace security
