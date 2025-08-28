#include "../../include/IO/DevLibTLSUtility.hpp"

#include <ostream>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/x509v3.h>

#include "../../include/IO/CFile.hpp"

namespace DevLib
{
	namespace IO
	{
		namespace TLS
		{

			// Root CA
			std::string ROOT_CA_ = R"(-----BEGIN CERTIFICATE-----
MIIC+TCCAeGgAwIBAgIBATANBgkqhkiG9w0BAQsFADAvMQswCQYDVQQGEwJLUjEP
MA0GA1UECgwGRGV2TGliMQ8wDQYDVQQDDAZSb290Q0EwHhcNMjUwNDE1MDIwNzA3
WhcNMzUwNDEzMDIwNzA3WjAvMQswCQYDVQQGEwJLUjEPMA0GA1UECgwGRGV2TGli
MQ8wDQYDVQQDDAZSb290Q0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIB
AQC7Dxje8l0efwVNqhETfv9SaK6O1dln5n5s6yxe9HO28Nrw01CEtYsha0kUnSKp
Ujxs+aSbCP/mzdlrLhgKmE0YhKwgu47ptHvAGN+8CEl6P/j1Me0Zg+9hXzmUDSD/
xBsDnviXXS4jR+HJpdwhCqZAFFkl2X4kduSGgKkT6gbU5RhxOIzvFD1RjrCtYl7L
+Gqk5JGzvbgpRD+yCKmkBs/2jXjwMGCq6TTJ0bRb/cIIhKc8typYQ6Lr13c02YC+
bx1yhDx+rpXL2oly/JOy5eGmAEci5iQY/b0sh1pU00AWusJkGzKm1ppX2CF/PR4f
J4AQX1VMijFzC9PYFuFgSqaHAgMBAAGjIDAeMA8GA1UdEwQIMAYBAf8CAQEwCwYD
VR0PBAQDAgEGMA0GCSqGSIb3DQEBCwUAA4IBAQAZqw6Y22WtF7s0AqsebXzGgx3/
AudPGaVDx69xXiMds/fgzZ9fE4AWpCwv5msoVB+uhf7Jn5SyXIffYXtS8LKwPRAk
zUxc/gSyEbi+NtCbxg/CGrNJ2ovqM/2UF/d15HFpgm3nRg2pC9J7FLu7yIrsWeWn
Bm7jbCyYUTW901T9Us2gXQsU7aRuslvOjHXVOBi1k1As4ZBdatc4NUbwb6Iviutv
gk9/Nvu8palC3c9wwX9HvnnodwpHoYUm7oGu5Fdq6naUI2felj6n3f7AvVAtne32
pPapoAs04A0FGqDUDmvuXPmUmDbwzFkPnletAHXpQjIthl/cAZYxLHuszGiK
-----END CERTIFICATE-----
)";

			// Root KEY
			std::string ROOT_KEY_ = R"(-----BEGIN PRIVATE KEY-----
MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQC7Dxje8l0efwVN
qhETfv9SaK6O1dln5n5s6yxe9HO28Nrw01CEtYsha0kUnSKpUjxs+aSbCP/mzdlr
LhgKmE0YhKwgu47ptHvAGN+8CEl6P/j1Me0Zg+9hXzmUDSD/xBsDnviXXS4jR+HJ
pdwhCqZAFFkl2X4kduSGgKkT6gbU5RhxOIzvFD1RjrCtYl7L+Gqk5JGzvbgpRD+y
CKmkBs/2jXjwMGCq6TTJ0bRb/cIIhKc8typYQ6Lr13c02YC+bx1yhDx+rpXL2oly
/JOy5eGmAEci5iQY/b0sh1pU00AWusJkGzKm1ppX2CF/PR4fJ4AQX1VMijFzC9PY
FuFgSqaHAgMBAAECggEAFZjAH9lum9tBcv6MGyqh8tNIOBCDmcXYWd3rEQl7JKd5
N63GEZRePF0FIn6j5lWqyVAnS4kF9pFUpVW8i9iaqhaDQwI0lVHzqbGVf0GbriQd
q714aOY9KIpUfP3pDe6Hyeq8O8mTzpn5fQ0Ltbrjh+lxg3exYydg6hCHtNdjs4eO
4taalPhlJw39kbefdpnkoR2hTIE05Is3XEht+ZoTqVM2Zo/CwT2g6jq6zYswOUp/
a+xdJHKLeF6m6av6qtzCj/yG6K5/WsalwGtruw6E6YE0dwzUu2whFTbNBX4J0LtN
GhwEJrVAuYBakNv705Bxj1zvr+x1qRBjFnl2G1UM7QKBgQD2ALWec38ci3PCDTr6
OE915rityoY9fr3uG6kQWtdqfSEhY9jvBKwR6DSqQ0fSvkj+xnOLhr7F1Eul9xNI
/xvZ+FNMgnMJj/hvD+jBkT6v0RTdQzNZCtmO7I3Q02Xjqslsu+9GtTub4jNjI3y8
VoynfFVRM4z+jvcI7eTq7oCFUwKBgQDCqSptHlJakBozVYcm7MPULY8XsDlpQBkk
SNn3GsxBepyUqKvqQ+OkTHRJPLoAqgH7X41kSwrDCw651afkQIU0Qsk5kNTyaxSm
LyMtE4TsKwWABqoWpbPWPN2zhERlNY06NEV9qAm/7UrhRGehfXxXTr+htwlb6b8n
whnaifmffQKBgAVFJp7qYoK+ChDSL71Ej487DiHLAO04VeFP/mM3Tv4dCCSq+u7D
7tvDWb18C4HcSbwLjFYk4NlkHurDTEp4DKSyyTYOZWCDERNwB9B9yBYEKKpE0kjK
DrcdIjyjCr/B3ObTb+DX6RsB0ZACJp3yTLkZ1MoUjKS3c1thnVHbIi25AoGBALQE
e07EDsbfoss8WiteI8+RflYay00RUO7kTGYfqysWt70mqy2PMPMdHSEpfp3kmsab
3AcW+tHa5PhopMYKAOWOqcaJsNSqwELbLr2d4nwBYSm/gAZHoSML0L8WH6Y1OvDL
rEQDZVjPmkLu3qNSh+25zrA/s/EDHpsm/n/Cv02FAoGBAJhL7dhTKsXz8eX4AtqQ
RFprOePjKAe4rj6e0F0t8kABkITfG/ZqK+Z/rAIMsUaQeeafmzhU4jEwKr/rPACe
Y7JJD99gfPov761BEAV8+XaS9+ThDT2UzCrccSM3LXn/7TSFbaRJguabaEB/70K6
GnqI4F64FWDvHK1vRcqXYfDq
-----END PRIVATE KEY-----
)";

			inline EVP_PKEY* PKEY_StringToPKEY(const std::string& privateKey)
			{
				// 문자열을 메모리 기반 BIO 객체로 변환
				BIO* bio = BIO_new_mem_buf(privateKey.data(), static_cast<int32_t>(privateKey.size()));

				EVP_PKEY* pKey = nullptr;
				if (bio)
				{
					// PEM 형식의 개인 키를 EVP_PKEY로 변환
					pKey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);

					// BIO 객체 해제
					BIO_free(bio);
				}

				return pKey;
			}

			inline std::string PKEY_PKEYToString(const EVP_PKEY* pKey)
			{
				std::string strKey;
				if (pKey)
				{
					// 메모리 기반 BIO 객체 생성
					BIO* bio = BIO_new(BIO_s_mem());
					if (bio)
					{
						// EVP_PKEY를 PEM 형식으로 BIO에 쓰기
						PEM_write_bio_PrivateKey(bio, pKey, nullptr, nullptr, 0, nullptr, nullptr);

						// BIO 데이터를 문자열로 읽기
						char* pem_data;
						const long pem_length = BIO_get_mem_data(bio, &pem_data);
						strKey = std::string(pem_data, pem_length);

						// BIO 객체 해제
						BIO_free(bio);
					}
				}

				return strKey;
			}

			inline X509_REQ* X509_StringToReq(const std::string& strCSR)
			{
				// 문자열을 메모리 기반 BIO 객체로 변환
				BIO* bio = BIO_new_mem_buf(strCSR.data(), static_cast<int32_t>(strCSR.size()));

				X509_REQ* pReq = nullptr;
				if (bio)
				{
					// PEM 형식의 개인 키를 X509_REQ로 변환
					pReq = PEM_read_bio_X509_REQ(bio, nullptr, nullptr, nullptr);

					// BIO 객체 해제
					BIO_free(bio);
				}

				return pReq;
			}


			inline std::string X509_ReqToString(const X509_REQ* req)
			{
				std::string strReq;

				if (req)
				{
					// 메모리 기반 BIO 객체 생성
					BIO* bio = BIO_new(BIO_s_mem());
					if (bio)
					{
						// X509_REQ를 PEM 형식으로 BIO에 쓰기
						if (PEM_write_bio_X509_REQ(bio, req) > 0)
						{
							// BIO 데이터를 문자열로 읽기
							char* pem_data;
							const long pem_length = BIO_get_mem_data(bio, &pem_data);
							strReq = std::string(pem_data, pem_length);
						}

						// BIO 객체 해제
						BIO_free(bio);
					}
				}

				return strReq;
			}

			inline X509* X509_StringToCert(const std::string& strCert)
			{
				// 문자열을 메모리 기반 BIO 객체로 변환
				BIO* bio = BIO_new_mem_buf(strCert.data(), static_cast<int32_t>(strCert.size()));

				X509* pReq = nullptr;
				if (bio)
				{
					// PEM 형식의 개인 키를 X509_REQ로 변환
					pReq = PEM_read_bio_X509(bio, nullptr, nullptr, nullptr);

					// BIO 객체 해제
					BIO_free(bio);
				}

				return pReq;
			}


			inline std::string X509_CertToString(const X509* cert)
			{
				std::string strCert;

				if (cert)
				{
					// 메모리 기반 BIO 객체 생성
					BIO* bio = BIO_new(BIO_s_mem());
					if (bio)
					{
						// X509_REQ를 PEM 형식으로 BIO에 쓰기
						if (PEM_write_bio_X509(bio, cert) > 0)
						{
							// BIO 데이터를 문자열로 읽기
							char* pem_data;
							const long pem_length = BIO_get_mem_data(bio, &pem_data);
							strCert = std::string(pem_data, pem_length);
						}

						// BIO 객체 해제
						BIO_free(bio);
					}
				}

				return strCert;
			}

			std::string GetRootCA()
			{
				return ROOT_CA_;
			}

			std::string GetRootKey()
			{
				return ROOT_KEY_;
			}

			bool SaveRootCAFile(const std::string& filePath)
			{
				bool bRet = false;

				DevLib::IO::CFile file;
				if (file.Open(filePath, "wt"))
				{
					if (file.WriteString(std::string(ROOT_CA_)) == ROOT_CA_.size())
					{
						bRet = true;
					}
				}

				return bRet;
			}

			std::string GeneratorPrivateKey(int32_t bits)
			{
				std::string strKey;

				// EVP_PKEY 생성 컨텍스트 초기화
				EVP_PKEY_CTX* pKeyCtx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);

				if (pKeyCtx)
				{
					if (EVP_PKEY_keygen_init(pKeyCtx) > 0)
					{
						if (EVP_PKEY_CTX_set_rsa_keygen_bits(pKeyCtx, bits) > 0)
						{
							// 키 생성
							EVP_PKEY* pKey = nullptr;
							if (EVP_PKEY_keygen(pKeyCtx, &pKey) > 0)
							{
								// 컨텍스트 정리
								EVP_PKEY_CTX_free(pKeyCtx);

								// EVP_PKEY to String
								strKey = PKEY_PKEYToString(pKey);

								// Key 해제
								EVP_PKEY_free(pKey);

							}
						}
					}
				}

				return strKey;
			}

			bool GeneratorPrivateKeyFile(const std::string& filePath, int32_t bits)
			{
				bool bRet = false;

				const auto key = GeneratorPrivateKey(bits);
				if (!key.empty())
				{
					DevLib::IO::CFile file;

					if (file.Open(filePath, "wt"))
					{
						if (file.WriteString(key) == key.size())
						{
							bRet = true;
						}
					}
				}

				return bRet;
			}

			std::string GeneratorCSR(const std::string& privateKey, const std::string& organization, const std::string& commonName)
			{
				std::string strCSR;
				X509_REQ* req = X509_REQ_new();
				if (req)
				{
					if (X509_REQ_set_version(req, 1) > 0)
					{
						EVP_PKEY* pKey = PKEY_StringToPKEY(privateKey);

						// 서버의 공개 키 설정
						if (X509_REQ_set_pubkey(req, pKey) > 0)
						{
							// subject 정보 설정: 이 예제에서는 Country, Organization, Common Name 필드 추가
							X509_NAME* name = X509_NAME_new();
							if (name)
							{
								// Country: KR
								X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, reinterpret_cast<const unsigned char*>("KR"), -1, -1, 0);
								// Organization
								X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, reinterpret_cast<const unsigned char*>(organization.c_str()), -1, -1, 0);
								// Common Name: 서버 도메인
								X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, reinterpret_cast<const unsigned char*>(commonName.c_str()), -1, -1, 0);

								if (X509_REQ_set_subject_name(req, name) > 0)
								{
									// CSR에 서명 (서명 알고리즘: SHA256)
									if (X509_REQ_sign(req, pKey, EVP_sha256()) > 0)
									{
										strCSR = X509_ReqToString(req);
									}
								}

								X509_NAME_free(name);
							}

						}
					}

					X509_REQ_free(req);

				}

				return strCSR;
			}

			bool GeneratorCSRFile(const std::string& filePath, const std::string& privateKey, const std::string& organization, const std::string& commonName)
			{
				bool bRet = false;

				const auto csr = GeneratorCSR(privateKey, organization, commonName);

				if (!csr.empty())
				{
					DevLib::IO::CFile file;

					if (file.Open(filePath, "wt"))
					{
						if (file.WriteString(csr) == csr.size())
						{
							bRet = true;
						}
					}
				}

				return bRet;
			}

			std::string GeneratorCertificate(const std::string& privateKey, const std::string& organization, const std::string& commonName, int32_t daysValid)
			{
				std::string strCert;
				X509* cert = X509_new();
				if (cert)
				{
					// X.509의 버전은 3 (0-indexed → 2)
					X509_set_version(cert, 2);

					// 간단한 시리얼 번호 설정 (실제 환경에서는 유일한 번호를 사용해야 함)
					//ASN1_INTEGER_set(X509_get_serialNumber(cert), 1);
					ASN1_INTEGER* serial = ASN1_INTEGER_new();
					BIGNUM* bn = BN_new();
					BN_rand(bn, 64, 0, 0); // 64비트 길이의 난수 생성
					BN_to_ASN1_INTEGER(bn, serial);
					X509_set_serialNumber(cert, serial);
					ASN1_INTEGER_free(serial);
					BN_free(bn);

					// 발급자(issuer)는 Root CA의 subject를 사용
					X509* rootCA = X509_StringToCert(std::string(ROOT_CA_));
					if (rootCA)
					{
						if (X509_set_issuer_name(cert, X509_get_subject_name(rootCA)) > 0)
						{
							// 주체(subject)를 Sub CA 정보로 설정
							X509_NAME* subject_name = X509_NAME_new();
							X509_NAME_add_entry_by_txt(subject_name, "C", MBSTRING_ASC, reinterpret_cast<const unsigned char*>("KR"), -1, -1, 0);
							X509_NAME_add_entry_by_txt(subject_name, "O", MBSTRING_ASC, reinterpret_cast<const unsigned char*>(organization.c_str()), -1, -1, 0);
							X509_NAME_add_entry_by_txt(subject_name, "CN", MBSTRING_ASC, reinterpret_cast<const unsigned char*>(commonName.c_str()), -1, -1, 0);
							if (X509_set_subject_name(cert, subject_name) > 0)
							{
								// 키 복사
								EVP_PKEY* pKey = PKEY_StringToPKEY(privateKey);

								if (X509_set_pubkey(cert, pKey))
								{
									// 유효기간 설정: 현재부터 daysValid 일 후까지
									ASN1_TIME* notBefore = ASN1_TIME_new();
									ASN1_TIME* notAfter = ASN1_TIME_new();
									X509_gmtime_adj(notBefore, 0);
									X509_gmtime_adj(notAfter, 60L * 60L * 24L * daysValid);
									X509_set_notBefore(cert, notBefore);
									X509_set_notAfter(cert, notAfter);
									ASN1_TIME_free(notBefore);
									ASN1_TIME_free(notAfter);

									// 기본 확장 추가: 기본 제약조건(basicConstraints): CA:FALSE
									X509_EXTENSION* ext = X509V3_EXT_conf_nid(nullptr, nullptr, NID_basic_constraints, "CA:TRUE, pathlen:0");
									if (ext)
									{
										X509_add_ext(cert, ext, -1);
										X509_EXTENSION_free(ext);
									}

									// 키 사용(Key Usage): digitalSignature, keyEncipherment
									ext = X509V3_EXT_conf_nid(nullptr, nullptr, NID_key_usage, "digitalSignature,keyEncipherment");
									if (ext)
									{
										X509_add_ext(cert, ext, -1);
										X509_EXTENSION_free(ext);
									}

									// 인증서를 Root CA 개인 키로 서명 (SHA256)
									EVP_PKEY* RootKey = PKEY_StringToPKEY(std::string(ROOT_KEY_));
									if (RootKey)
									{
										if (X509_sign(cert, RootKey, EVP_sha256()) > 0)
										{
											strCert = X509_CertToString(cert);
										}

										EVP_PKEY_free(RootKey);
									}
								}

								EVP_PKEY_free(pKey);
							}

							X509_NAME_free(subject_name);
						}

						X509_free(rootCA);
					}

					X509_free(cert);
				}

				return strCert;
			}

			bool GeneratorCertificateFile(const std::string& filePath, const std::string& privateKey, const std::string& organization, const std::string& commonName, int32_t daysValid)
			{
				bool bRet = false;

				const auto cert = GeneratorCertificate(privateKey, organization, commonName, daysValid);

				if (!cert.empty())
				{
					DevLib::IO::CFile file;

					if (file.Open(filePath, "wt"))
					{
						if (file.WriteString(cert) == cert.size())
						{
							bRet = true;
						}
					}
				}

				return bRet;
			}

			std::string GeneratorCertificateFromCSR(const std::string& csr, int32_t daysValid)
			{
				std::string strCert;
				X509* cert = X509_new();
				if (cert)
				{
					// X.509의 버전은 3 (0-indexed → 2)
					X509_set_version(cert, 2);

					// 간단한 시리얼 번호 설정 (실제 환경에서는 유일한 번호를 사용해야 함)
					//ASN1_INTEGER_set(X509_get_serialNumber(cert), 1);
					ASN1_INTEGER* serial = ASN1_INTEGER_new();
					BIGNUM* bn = BN_new();
					BN_rand(bn, 64, 0, 0); // 64비트 길이의 난수 생성
					BN_to_ASN1_INTEGER(bn, serial);
					X509_set_serialNumber(cert, serial);
					ASN1_INTEGER_free(serial);
					BN_free(bn);

					// 발급자(issuer)는 Root CA의 subject를 사용
					X509* rootCA = X509_StringToCert(std::string(ROOT_CA_));
					if (rootCA)
					{
						const X509_NAME* issuer = X509_get_subject_name(rootCA);
						if (X509_set_issuer_name(cert, issuer) > 0)
						{
							X509_REQ* req = X509_StringToReq(csr);
							if (req)
							{
								// 주체(subject)는 CSR의 subject를 그대로 사용
								const X509_NAME* subj = X509_REQ_get_subject_name(req);
								if (X509_set_subject_name(cert, subj) > 0)
								{
									// CSR의 공개 키 복사
									EVP_PKEY* req_pkey = X509_REQ_get_pubkey(req);
									if (req_pkey)
									{
										if (X509_set_pubkey(cert, req_pkey) > 0)
										{
											if (X509_set_pubkey(cert, req_pkey))
											{
												// 유효기간 설정: 현재부터 daysValid 일 후까지
												ASN1_TIME* notBefore = ASN1_TIME_new();
												ASN1_TIME* notAfter = ASN1_TIME_new();
												X509_gmtime_adj(notBefore, 0);
												X509_gmtime_adj(notAfter, 60L * 60L * 24L * daysValid);
												X509_set_notBefore(cert, notBefore);
												X509_set_notAfter(cert, notAfter);
												ASN1_TIME_free(notBefore);
												ASN1_TIME_free(notAfter);

												// 기본 확장 추가: 기본 제약조건(basicConstraints): CA:FALSE
												X509_EXTENSION* ext = X509V3_EXT_conf_nid(nullptr, nullptr, NID_basic_constraints, "CA:TRUE, pathlen:0");
												if (ext)
												{
													X509_add_ext(cert, ext, -1);
													X509_EXTENSION_free(ext);
												}

												// 키 사용(Key Usage): digitalSignature, keyEncipherment
												ext = X509V3_EXT_conf_nid(nullptr, nullptr, NID_key_usage, "digitalSignature,keyEncipherment");
												if (ext)
												{
													X509_add_ext(cert, ext, -1);
													X509_EXTENSION_free(ext);
												}

												// 인증서를 Root CA 개인 키로 서명 (SHA256)
												EVP_PKEY* RootKey = PKEY_StringToPKEY(std::string(ROOT_KEY_));
												if (RootKey)
												{
													if (X509_sign(cert, RootKey, EVP_sha256()) > 0)
													{
														strCert = X509_CertToString(cert);
													}

													EVP_PKEY_free(RootKey);
												}
											}
										}

										EVP_PKEY_free(req_pkey);
									}
								}

								X509_REQ_free(req);
							}
						}

						X509_free(rootCA);
					}

					X509_free(cert);
				}

				return strCert;
			}

			bool GeneratorCertificateFromCSRFile(const std::string& filePath, const std::string& csr, int32_t daysValid)
			{
				bool bRet = false;

				const auto cert = GeneratorCertificateFromCSR(csr, daysValid);

				if (!cert.empty())
				{
					DevLib::IO::CFile file;

					if (file.Open(filePath, "wt"))
					{
						if (file.WriteString(cert) == cert.size())
						{
							bRet = true;
						}
					}
				}

				return bRet;
			}

			std::string GeneratorCertificateSub(const std::string& privateKey, const std::string& caCert, const std::string& caKey, const std::string& organization, const std::string& commonName, int32_t daysValid)
			{
				std::string strCert;
				X509_REQ* pReq = X509_StringToReq(GeneratorCSR(privateKey, organization, commonName));
				if (pReq)
				{
					X509* subCert = X509_new();
					if (subCert)
					{
						// X.509의 버전은 3 (0-indexed → 2)
						X509_set_version(subCert, 2);

						// 간단한 시리얼 번호 설정 
						//ASN1_INTEGER_set(X509_get_serialNumber(cert), 1);
						ASN1_INTEGER* serial = ASN1_INTEGER_new();
						BIGNUM* bn = BN_new();
						BN_rand(bn, 64, 0, 0); // 64비트 길이의 난수 생성
						BN_to_ASN1_INTEGER(bn, serial);
						X509_set_serialNumber(subCert, serial);
						ASN1_INTEGER_free(serial);
						BN_free(bn);

						// 유효기간 설정: 현재부터 daysValid 일 후까지
						ASN1_TIME* notBefore = ASN1_TIME_new();
						ASN1_TIME* notAfter = ASN1_TIME_new();
						X509_gmtime_adj(notBefore, 0);
						X509_gmtime_adj(notAfter, 60L * 60L * 24L * daysValid);
						X509_set_notBefore(subCert, notBefore);
						X509_set_notAfter(subCert, notAfter);
						ASN1_TIME_free(notBefore);
						ASN1_TIME_free(notAfter);

						// 발급자(issuer)는 Root CA의 subject를 사용
						X509* rootCa = X509_StringToCert(caCert);
						if (rootCa)
						{

							EVP_PKEY* subKey = PKEY_StringToPKEY(privateKey);
							EVP_PKEY* rootKey = PKEY_StringToPKEY(caKey);

							X509_set_subject_name(subCert, X509_REQ_get_subject_name(pReq));
							X509_set_issuer_name(subCert, X509_get_subject_name(rootCa)); // Root CA 인증서에서 발급자 설정
							X509_set_pubkey(subCert, subKey);

							if (X509_sign(subCert, rootKey, EVP_sha256()) > 0)
							{
								strCert = X509_CertToString(subCert);
							}

							EVP_PKEY_free(subKey);
							EVP_PKEY_free(rootKey);

							X509_free(rootCa);
						}

						X509_free(subCert);
					}

					X509_REQ_free(pReq);
				}

				return strCert;
			}

			bool GeneratorCertificateSubFile(const std::string& filePath, const std::string& privateKey, const std::string& caCert, const std::string& caKey, const std::string& organization, const std::string& commonName, int32_t daysValid)
			{
				bool bRet = false;

				const auto cert = GeneratorCertificateSub(privateKey, caCert, caKey, organization, commonName, daysValid);

				if (!cert.empty())
				{
					DevLib::IO::CFile file;

					if (file.Open(filePath, "wt"))
					{
						if (file.WriteString(cert) == cert.size())
						{
							bRet = true;
						}
					}
				}

				return bRet;
			}

			std::string GeneratorCertificateSelf(const std::string& privateKey, const std::string& organization, const std::string& commonName, int32_t daysValid)
			{
				std::string strCert;

				EVP_PKEY* pKey = PKEY_StringToPKEY(privateKey);

				if (pKey)
				{
					X509* cert = X509_new();
					if (cert)
					{
						// X.509의 버전은 3 (0-indexed → 2)
						X509_set_version(cert, 2);

						// 간단한 시리얼 번호 설정 (실제 환경에서는 유일한 번호를 사용해야 함)
						ASN1_INTEGER_set(X509_get_serialNumber(cert), 1);

						// 유효기간 설정: 현재부터 daysValid 일 후까지
						ASN1_TIME* notBefore = ASN1_TIME_new();
						ASN1_TIME* notAfter = ASN1_TIME_new();
						X509_gmtime_adj(notBefore, 0);
						X509_gmtime_adj(notAfter, 60L * 60L * 24L * daysValid);
						X509_set_notBefore(cert, notBefore);
						X509_set_notAfter(cert, notAfter);
						ASN1_TIME_free(notBefore);
						ASN1_TIME_free(notAfter);

						// 공개 키 설정
						if (X509_set_pubkey(cert, pKey) > 0)
						{
							// 주체(subject)와 발급자(issuer) 이름 설정
							X509_NAME* name = X509_NAME_new();
							if (name)
							{
								// 주체(subject) 이름 설정
								X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, reinterpret_cast<const unsigned char*>("KR"), -1, -1, 0);
								X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, reinterpret_cast<const unsigned char*>(organization.c_str()), -1, -1, 0);
								X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, reinterpret_cast<const unsigned char*>(commonName.c_str()), -1, -1, 0);

								// 인증서의 주체와 발급자(issuer)를 동일하게 설정
								if (X509_set_subject_name(cert, name) > 0)
								{
									if (X509_set_issuer_name(cert, name) > 0)
									{
										// 기본 확장 추가: 기본 제약조건(basicConstraints): CA:FALSE
										X509_EXTENSION* ext = X509V3_EXT_conf_nid(nullptr, nullptr, NID_basic_constraints, "CA:TRUE, pathlen:1");
										if (ext)
										{
											X509_add_ext(cert, ext, -1);
											X509_EXTENSION_free(ext);
										}

										// 키 사용(Key Usage): digitalSignature, keyEncipherment
										ext = X509V3_EXT_conf_nid(nullptr, nullptr, NID_key_usage, "keyCertSign, cRLSign");
										if (ext)
										{
											X509_add_ext(cert, ext, -1);
											X509_EXTENSION_free(ext);
										}

										// 인증서에 자체 서명 (Self-Signed, SHA256 사용)
										if (X509_sign(cert, pKey, EVP_sha256()))
										{
											strCert = X509_CertToString(cert);
										}
									}
								}
							}

							X509_NAME_free(name);
						}

						X509_free(cert);
					}

					EVP_PKEY_free(pKey);
				}

				return strCert;
			}

			bool GeneratorCertificateSelfFile(const std::string& filePath, const std::string& privateKey, const std::string& organization, const std::string& commonName, int32_t daysValid)
			{
				bool bRet = false;

				const auto cert = GeneratorCertificateSelf(privateKey, organization, commonName, daysValid);

				if (!cert.empty())
				{
					DevLib::IO::CFile file;

					if (file.Open(filePath, "wt"))
					{
						if (file.WriteString(cert) == cert.size())
						{
							bRet = true;
						}
					}
				}

				return bRet;

			}

			bool VerifyCertificate(const std::string& strCert)
			{
				bool bRet = false;

				X509_STORE* store = X509_STORE_new();
				if (store)
				{
					X509* RootCA = X509_StringToCert(std::string(ROOT_CA_));
					if (RootCA)
					{
						if (X509_STORE_add_cert(store, RootCA) == 1)
						{
							// 검증 컨텍스트 생성
							X509_STORE_CTX* ctx = X509_STORE_CTX_new();
							if (ctx)
							{
								// 검증할 인증서 로드
								X509* cert = X509_StringToCert(strCert);
								if (cert)
								{
									// 컨텍스트 초기화
									if (X509_STORE_CTX_init(ctx, store, cert, nullptr) == 1)
									{
										// 인증서 검증 실행
										if (X509_verify_cert(ctx) == 1)
										{
											bRet = true;
										}
									}

									X509_free(cert);
								}

								X509_STORE_CTX_free(ctx);
							}
						}

						X509_free(RootCA);
					}

					X509_STORE_free(store);
				}

				return bRet;
			}

			bool VerifyCertificate(const std::string& strCA, const std::string& strCert)
			{
				bool bRet = false;

				X509_STORE* store = X509_STORE_new();
				if (store)
				{
					X509* RootCA = X509_StringToCert(strCA);
					if (RootCA)
					{
						if (X509_STORE_add_cert(store, RootCA) == 1)
						{
							// 검증 컨텍스트 생성
							X509_STORE_CTX* ctx = X509_STORE_CTX_new();
							if (ctx)
							{
								// 검증할 인증서 로드
								X509* cert = X509_StringToCert(strCert);
								if (cert)
								{
									// 컨텍스트 초기화
									if (X509_STORE_CTX_init(ctx, store, cert, nullptr) == 1)
									{
										// 인증서 검증 실행
										if (X509_verify_cert(ctx) == 1)
										{
											bRet = true;
										}
									}

									X509_free(cert);
								}

								X509_STORE_CTX_free(ctx);
							}
						}

						X509_free(RootCA);
					}

					X509_STORE_free(store);
				}

				return bRet;
			}
		}
	}
}