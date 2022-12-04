#pragma once

# include "common.hpp"

typedef struct s_err_100
{
	std::map<int, std::string>	errs;

	s_err_100 ()
	{
		errs.insert(std::make_pair(100, "Continue")),
		errs.insert(std::make_pair(101, "Switching Protocol"));
		errs.insert(std::make_pair(102, "Processing"));
		errs.insert(std::make_pair(103, "Early Hints"));
	}

	std::string	operator[](int n)
	{
		if (n >= 100)
			return (errs[n]);
		return (errs[100 + n]);
	}
} t_err100;

typedef struct s_ret_200
{
	std::map<int, std::string>	ret;

	s_ret_200 ()
	{
		ret.insert(std::make_pair(200, "OK")),
		ret.insert(std::make_pair(201, "Created"));
		ret.insert(std::make_pair(202, "Accepted"));
		ret.insert(std::make_pair(203, "Non-Authoritative Information"));
		ret.insert(std::make_pair(204, "No Content"));
		ret.insert(std::make_pair(205, "Reset Content"));
		ret.insert(std::make_pair(206, "Partial Content"));
		ret.insert(std::make_pair(207, "Multi-Status"));
		ret.insert(std::make_pair(208, "Already Reported"));
		ret.insert(std::make_pair(210, "Content Different"));
		ret.insert(std::make_pair(226, "IM Used"));
	}

	std::string	operator[](int n)
	{
		if (n >= 200)
			return (ret[n]);
		return (ret[200 + n]);
	}
} t_ret200;

typedef struct s_err_300
{
	std::map<int, std::string>	errs;

	s_err_300 ()
	{
		errs.insert(std::make_pair(300, "Multiple Choices")),
		errs.insert(std::make_pair(301, "Moved Permanently"));
		errs.insert(std::make_pair(302, "Found"));
		errs.insert(std::make_pair(303, "See Other"));
		errs.insert(std::make_pair(304, "Not Modified"));
		errs.insert(std::make_pair(305, "Use Proxy"));
		errs.insert(std::make_pair(307, "Temporary Redirect"));
		errs.insert(std::make_pair(308, "Permanent Redirect"));
		errs.insert(std::make_pair(310, "Too Many Redirects"));
	}

	std::string	operator[](int n)
	{
		if (n >= 300)
			return (errs[n]);
		return (errs[300 + n]);
	}
} t_err300;

typedef struct s_err_400
{
	std::map<int, std::string>	errs;

	s_err_400()
	{
		errs.insert(std::make_pair(400, "Bad Request")),
		errs.insert(std::make_pair(401, "Unauthorized"));
		errs.insert(std::make_pair(402, "Payment Required"));
		errs.insert(std::make_pair(403, "Forbidden"));
		errs.insert(std::make_pair(404, "Not Found"));
		errs.insert(std::make_pair(405, "Method Not Allowed"));
		errs.insert(std::make_pair(406, "Not Acceptable"));
		errs.insert(std::make_pair(407, "Proxy Authentication Requiered"));
		errs.insert(std::make_pair(408, "Request Time-out"));
		errs.insert(std::make_pair(409, "Conflict"));
		errs.insert(std::make_pair(410, "Gone"));
		errs.insert(std::make_pair(411, "Length Required"));
		errs.insert(std::make_pair(412, "Precondition Failed"));
		errs.insert(std::make_pair(413, "Request Entity Too Large"));
		errs.insert(std::make_pair(414, "Request-URI Too Long"));
		errs.insert(std::make_pair(415, "Unsuported Media Type"));
		errs.insert(std::make_pair(416, "Request range unsastifiable"));
		errs.insert(std::make_pair(417, "Expectation failed"));
		errs.insert(std::make_pair(421, "Misdirected Request"));
		errs.insert(std::make_pair(422, "Unprocessable entity"));
		errs.insert(std::make_pair(423, "Locked"));
		errs.insert(std::make_pair(424, "Failed Dependancy"));
		errs.insert(std::make_pair(425, "Unordered Collection"));
		errs.insert(std::make_pair(426, "Upgrade Requiered"));
		errs.insert(std::make_pair(428, "Precondition Requiered"));
		errs.insert(std::make_pair(429, "Too Many Requests"));
		errs.insert(std::make_pair(431, "Request Header Fields Too Large"));
		errs.insert(std::make_pair(449, "Retry With"));
		errs.insert(std::make_pair(450, "Blocked by Windows Parental Controls"));
		errs.insert(std::make_pair(451, "Unavailable For Legal Reasons"));
		errs.insert(std::make_pair(456, "Unrecoverable Error"));
	}

	std::string	operator[](int n)
	{
		if (n >= 400)
			return (errs[n]);
		return (errs[400 + n]);
	}
} t_err400;

typedef struct s_err_500
{
	std::map<int, std::string>	errs;

	s_err_500()
	{
		errs.insert(std::make_pair(500, "Internal Server Error")),
		errs.insert(std::make_pair(501, "Not Implemented"));
		errs.insert(std::make_pair(502, "Bad Gateway"));
		errs.insert(std::make_pair(503, "Service Unavailable"));
		errs.insert(std::make_pair(504, "Gateway Time-out"));
		errs.insert(std::make_pair(505, "HTTP Version not Supported"));
		errs.insert(std::make_pair(506, "Variant Also Negotiates"));
		errs.insert(std::make_pair(507, "Insufficient storage"));
		errs.insert(std::make_pair(508, "Loop detected"));
		errs.insert(std::make_pair(509, "Bandwidth Limit Exceeded"));
		errs.insert(std::make_pair(510, "Not extended"));
		errs.insert(std::make_pair(511, "Network authentication requiered"));
	}

	std::string	operator[](int n)
	{
		if (n >= 500)
			return (errs[n]);
		return (errs[500 + n]);
	}
} t_err500;
