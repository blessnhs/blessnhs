
#pragma once

#include <sstream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>


class Gzip 
{
public:
	static std::string compress(const std::string& data)
	{
		namespace bio = boost::iostreams;

		std::stringstream compressed;
		std::stringstream origin(data);

		bio::filtering_streambuf<bio::input> out;
		out.push(bio::gzip_compressor(bio::gzip_params(bio::gzip::best_compression)));
		out.push(origin);
		bio::copy(out, compressed);

		return compressed.str();
	}

	static void compress(std::ostream& data)
	{
		namespace bio = boost::iostreams;

		std::stringstream origin;
		origin << data.rdbuf();

		bio::filtering_streambuf<bio::input> out;
		out.push(bio::gzip_compressor(bio::gzip_params(bio::gzip::best_compression)));
		out.push(origin);

		data.flush();

		bio::copy(out, data);
	}

	static std::string decompress(const std::string& data)
	{
		namespace bio = boost::iostreams;

		std::stringstream compressed(data);
		std::stringstream decompressed;

		bio::filtering_streambuf<bio::input> out;
		out.push(bio::gzip_decompressor());
		out.push(compressed);
		bio::copy(out, decompressed);

		return decompressed.str();
	}

	static void decompress(std::ostream& data)
	{
		namespace bio = boost::iostreams;

		std::stringstream decompressed;
		decompressed << data.rdbuf();

		bio::filtering_streambuf<bio::input> out;
		out.push(bio::gzip_decompressor());
		out.push(decompressed);

		data.flush();

		bio::copy(out, data);
	}

};
