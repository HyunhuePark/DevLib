#pragma once

#include "../Base/DevLibTypes.hpp"
#include <map>

namespace DevLib
{
	namespace IO
	{
		namespace TLS
		{
			std::string GetRootCA();
			std::string GetRootKey();

			bool SaveRootCAFile(const std::string& filePath);

			std::string GeneratorPrivateKey(int32_t bits = 2048);
			bool GeneratorPrivateKeyFile(const std::string& filePath, int32_t bits = 2048);

			std::string GeneratorCSR(const std::string& privateKey, const std::string& organization, const std::string& commonName);
			bool GeneratorCSRFile(const std::string& filePath, const std::string& privateKey, const std::string& organization, const std::string& commonName);

			// DevLib Certification
			std::string GeneratorCertificate(const std::string& privateKey, const std::string& organization, const std::string& commonName, int32_t daysValid);
			bool GeneratorCertificateFile(const std::string& filePath, const std::string& privateKey, const std::string& organization, const std::string& commonName, int32_t daysValid);

			std::string GeneratorCertificateFromCSR(const std::string& csr, int32_t daysValid);
			bool GeneratorCertificateFromCSRFile(const std::string& filePath, const std::string& csr, int32_t daysValid);

			// Sub Certification
			std::string GeneratorCertificateSub(const std::string& privateKey, const std::string& caCert, const std::string& caKey, const std::string& organization, const std::string& commonName, int32_t daysValid);
			bool GeneratorCertificateSubFile(const std::string& filePath, const std::string& privateKey, const std::string& caCert, const std::string& caKey, const std::string& organization, const std::string& commonName, int32_t daysValid);

			// Self Certification
			std::string GeneratorCertificateSelf(const std::string& privateKey, const std::string& organization, const std::string& commonName, int32_t daysValid);
			bool GeneratorCertificateSelfFile(const std::string& filePath, const std::string& privateKey, const std::string& organization, const std::string& commonName, int32_t daysValid = 365);

			// Verify DevLib
			bool VerifyCertificate(const std::string& strCert);

			// Verify CA
			bool VerifyCertificate(const std::string& strCA, const std::string& strCert);
		}
	}
}
