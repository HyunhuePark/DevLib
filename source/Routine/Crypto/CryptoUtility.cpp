#include "../../../include/Routine/Crypto/CryptoUtility.hpp"

#include "../../../include/Base/CScopeLocker.hpp"
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include <cryptopp/osrng.h>
#include <cryptopp/aes.h>
#include <cryptopp/ccm.h>
#include <cryptopp/md5.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>

namespace DevLib {
	namespace Routine
	{
		namespace Crypto
		{
			std::vector<uint8_t> GeneratorKey()
			{
				CryptoPP::AutoSeededRandomPool rnd;
				std::vector<uint8_t> key(CryptoPP::AES::MAX_KEYLENGTH);
				rnd.GenerateBlock(key.data(), CryptoPP::AES::MAX_KEYLENGTH);

				return key;
			}

			std::vector<uint8_t> GeneratorInitialVector()
			{
				CryptoPP::AutoSeededRandomPool rnd;
				std::vector<uint8_t> iv(CryptoPP::AES::BLOCKSIZE);
				rnd.GenerateBlock(iv.data(), CryptoPP::AES::BLOCKSIZE);

				return iv;
			}

			bool EncryptionAES(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv, const uint8_t* pSrc, size_t srcSize, std::vector<uint8_t>& dest)
			{
				bool bRet = true;

				try
				{
					CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption enc;
					enc.SetKeyWithIV(key.data(), key.size(), iv.data(), iv.size());

					dest.resize(srcSize + CryptoPP::AES::BLOCKSIZE - (srcSize % CryptoPP::AES::BLOCKSIZE));

					CryptoPP::ArraySink encSink(dest.data(), dest.size());
					CryptoPP::ArraySource encSource(pSrc, srcSize, true, new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(encSink)));

					dest.resize(encSink.TotalPutLength());
				}
				catch (const CryptoPP::Exception&)
				{
					bRet = false;
				}

				return bRet;
			}

			bool EncryptionAES(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv, const std::vector<uint8_t>& src, std::vector<uint8_t>& dest)
			{
				return EncryptionAES(key, iv, src.data(), src.size(), dest);
			}

			bool DecryptionAES(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv, const uint8_t* pSrc, size_t srcSize, std::vector<uint8_t>& dest)
			{
				bool bRet = true;

				try
				{
					CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption dec;
					dec.SetKeyWithIV(key.data(), key.size(), iv.data(), iv.size());

					dest.resize(srcSize + CryptoPP::AES::BLOCKSIZE - (srcSize % CryptoPP::AES::BLOCKSIZE));

					CryptoPP::ArraySink decSink(dest.data(), dest.size());
					CryptoPP::ArraySource decSource(pSrc, srcSize, true, new CryptoPP::StreamTransformationFilter(dec, new CryptoPP::Redirector(decSink)));

					dest.resize(decSink.TotalPutLength());
				}
				catch (const CryptoPP::Exception&)
				{
					bRet = false;
				}

				return bRet;
			}

			bool DecryptionAES(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv, const std::vector<uint8_t>& src, std::vector<uint8_t>& dest)
			{
				return DecryptionAES(key, iv, src.data(), src.size(), dest);
			}

			std::vector<uint8_t> SHA256(const std::vector<uint8_t>& in, bool bUppercase)
			{
				CryptoPP::SHA256 hash;

				std::vector<uint8_t> output(CryptoPP::SHA256::DIGESTSIZE);
				hash.CalculateDigest(output.data(), in.data(), in.size());

				return output;
			}

			std::vector<uint8_t> SHA256(const std::string& in, bool bUppercase)
			{
				std::vector<uint8_t> temp(in.size());
				std::copy(in.begin(), in.end(), temp.data());

				return SHA256(temp, bUppercase);
			}

			std::vector<uint8_t> SHA512(const std::vector<uint8_t>& in, bool bUppercase)
			{
				CryptoPP::SHA512 hash;

				std::vector<uint8_t> output(CryptoPP::SHA512::DIGESTSIZE);
				hash.CalculateDigest(output.data(), in.data(), in.size());

				return output;
			}

			std::vector<uint8_t> SHA512(const std::string& in, bool bUppercase)
			{
				std::vector<uint8_t> temp(in.size());
				std::copy(in.begin(), in.end(), temp.data());

				return SHA512(temp, bUppercase);
			}

			std::vector<uint8_t> MD5(const std::vector<uint8_t>& in, bool bUppercase)
			{
				CryptoPP::Weak1::MD5 hash;

				std::vector<uint8_t> output(CryptoPP::Weak1::MD5::DIGESTSIZE);
				hash.CalculateDigest(output.data(), in.data(), in.size());

				return output;
			}

			std::vector<uint8_t> MD5(const std::string& in, bool bUppercase)
			{
				std::vector<uint8_t> temp(in.size());
				std::copy(in.begin(), in.end(), temp.data());

				return MD5(temp, bUppercase);
			}

			std::string HexDataToString(const std::vector<uint8_t>& hexData, bool bUppercase)
			{
				std::string out;

				CryptoPP::HexEncoder encoder(nullptr, bUppercase);
				encoder.Attach(new CryptoPP::StringSink(out));
				encoder.Put(hexData.data(), hexData.size());
				encoder.MessageEnd();

				return out;
			}

			std::vector<uint8_t> HexStringToData(const std::string& hexString, bool bUppercase)
			{
				std::vector<uint8_t> out(hexString.size() / 2);

				CryptoPP::HexDecoder decoder;
				CryptoPP::ArraySink as(out.data(), out.size());
				decoder.Attach(new CryptoPP::Redirector(as));
				decoder.Put(reinterpret_cast<uint8_t*>(const_cast<char*>(hexString.data())), hexString.size());
				decoder.MessageEnd();

				return out;
			}
		}

	}
}
