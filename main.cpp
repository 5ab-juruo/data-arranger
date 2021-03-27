// Data arranger for competitive programming
// An easier way to debug your codes

// Version: 0.1 on Windows

#include <bits/stdc++.h>
using namespace std;

const string echo_off = "echo off\n";
const string mkdir = "mkdir";
const string cd = "cd";
const string notepad = "notepad";
const string tmpdir_name = "datas";
const char slh = '\\';

const string cmd_help = 
R"("test": Test your program(compile required);
"add": Add a new set of data to your archive;
"clear": Clear all your datas;
"help": Display this guide again;
"quit": Quit.)";

struct f_cfg
{
	int ids;
};

stringstream ss;
fstream fs;
vector<string> in_args;

bool is_quit = false;

void trunc_str(string& str)
{
	while (str.size() > 0 && isspace(str[str.size()-1]))
		str.pop_back();
}

int run_cmd(string cmd)
{
//	cerr << "Trying to run \"" << cmd << "\"" << endl;
	cmd = string("cmd /c ") + cmd;
	return system(cmd.c_str());
}

int execute_prog(const string& prog_name, const string& in_file, const string& out_file)
{
	ss.str("");
	ss << "\"" << prog_name << "\" 0<\"" << in_file << "\" 1>\"" << out_file << "\"";
	return run_cmd(ss.str());
}

void new_file(const string& file_dir)
{
	fs.clear();
	fs.open(file_dir, ios::out | ios::trunc);
	if (fs.fail())
		throw runtime_error("Unable to create files.");
	fs.close();
}

void new_txt(const string& txt_dir)
{
	new_file(txt_dir);
	if (system((notepad + " \"" + txt_dir + "\"").c_str()))
		throw runtime_error("Error occurs while typing datas.");
}

void new_dir(const string& dir_name)
{
	ss.str("");
	
//	cerr << "Trying to visit " << dir_name << endl;

	ss << cd << " \"" << dir_name << "\"";
	if (!system(ss.str().c_str()))
		throw 114514;
	
//	cerr << "No directory exists. trying to create " << dir_name << endl;
	
	ss.str("");
	ss << mkdir << " \"" << dir_name << "\"";
	if (system(ss.str().c_str()))
		throw runtime_error("Unable to create directory.");
}

void rewrite_cfg(const string& file_dir, const f_cfg& cfg)
{
	fs.open(file_dir, ios::out | ios::trunc | ios::binary);
	fs.write((char*)&cfg, sizeof cfg);
	fs.close();
}

void ensure_dir(string& file_name)
{
	if (file_name[0] == '\"' && file_name[file_name.size()-1] == '\"')
	{
		file_name.erase(file_name.begin());
		file_name.erase(file_name.size()-1, 1);
	}
	else if (file_name[0] == '\'' && file_name[file_name.size()-1] == '\'')
	{
		file_name.erase(file_name.begin());
		file_name.erase(file_name.size()-1, 1);
	}

	if (file_name[file_name.size()-1] == slh)
		file_name.pop_back();
}

void cmp_outs(const string& std_out, const string& usr_out)
{
	ifstream stdf, usrf;
	string s1, s2;

	stdf.open(std_out, ios::in);
	usrf.open(usr_out, ios::in);

	while (!stdf.eof() && !usrf.eof())
	{
		getline(stdf, s1);
		getline(usrf, s2);

		trunc_str(s1);
		trunc_str(s2);

		if (s1 != s2)
		{
		//	cerr << "s1: \"" << s1 << "\", s2: \"" << s2 << "\"" << endl;
			throw logic_error("wrong answer.");
		}
	}

	if (usrf.eof())
	{
		while (!stdf.eof())
		{
			getline(stdf, s1), trunc_str(s1);
			if (s1.size() > 0)
				throw logic_error("unexpected eof.");
		}
		return;
	}

	if (stdf.eof())
	{
		while (!usrf.eof())
		{
			getline(usrf, s1), trunc_str(s1);
			if (s1.size() > 0)
				throw logic_error("extra output found.");
		}
	}

	return;
}

int main()
{
	ios_base::sync_with_stdio(false);
	cin.tie(0);

	string file_name, data_dirname, tmp_cmd, tmp_dname;
	f_cfg cfg;
	bool at_flag;

	while (!is_quit)
	{
		cout << "Select file: " << endl;
		getline(cin, file_name);

		ensure_dir(file_name);

		data_dirname = file_name;
		while (data_dirname[data_dirname.size()-1] != slh)
			data_dirname.pop_back();
		data_dirname += tmpdir_name;

	//	cerr << "Trying to create \"" << data_dirname << "\"" << endl;

		at_flag = false;
		try {
			new_dir(data_dirname);
		}
		catch (exception& e)
		{
			cout << "Exception occurs: " << e.what() << endl;
			return 1;
		}
		catch (...)
		{
			at_flag = true;
			fs.open(data_dirname + slh + "cfg", ios::in | ios::binary);
			fs.read((char*)&cfg, sizeof cfg);
		}

		if (!at_flag)
		{
			fs.open(data_dirname + slh + "cfg", ios::out | ios::binary);
			cfg.ids = 0;
			fs.write((char*)&cfg, sizeof cfg);
		}

		fs.close();

		cout << "Get started!" << endl;
		cout << cmd_help << endl;
		cout << "Enter your commands:" << endl;

		while (!is_quit)
		{
			cout << ">>> " << flush;
			getline(cin, tmp_cmd);
		//	cerr << "Successfully read command \"" << tmp_cmd << "\"" << endl;

			in_args.clear();

			auto pt = tmp_cmd.begin();
			while (pt != tmp_cmd.end())
			{
				if (!isspace(*pt))
					break;
				pt++;
			}
			
			while (pt != tmp_cmd.end())
			{
				in_args.push_back("");

				while (pt != tmp_cmd.end() && !isspace(*pt))
					in_args[in_args.size()-1] += *(pt++);
				while (pt != tmp_cmd.end())
				{
					if (!isspace(*pt))
						break;
					pt++;
				}
			}
			
		//	cerr << "Successfully get command \"" << tmp_cmd << "\"" << endl;

			if (in_args[0] == "add")
			{
				if (in_args.size() == 1)
				{
					cfg.ids++;

					ss.str("");
					ss << data_dirname << slh << cfg.ids << ".";
					tmp_dname = ss.str();
					
					try {
						new_txt(tmp_dname + "in");
						new_txt(tmp_dname + "out");
					}
					catch (exception& e)
					{
						cout << "Error: " << e.what() << endl;
					}

					rewrite_cfg(data_dirname + slh + "cfg", cfg);
				}
				else
					cout << "Error: Too many arguments for command \"add\"." << endl;
			}
			else if (in_args[0] == "quit")
			{
				if (in_args.size() == 1)
				{
					cout << "Do you want to quit? (Y/N, case sensitive) " << flush;
					getline(cin, tmp_cmd);

					if (tmp_cmd == "Y")
						is_quit = true;
					else if (tmp_cmd == "N")
						cout << "Well...let's continue." << endl;
					else
						cout << "Invalid input." << endl;
				}
				else if (in_args.size() == 2)
				{
					if (in_args[1] == "-f")
						is_quit = true;
					else
						cout << "Invalid flags." << endl;
				}
				else
					cout << "Error: Too many arguments for command \"quit\"" << endl;
			}
			else if (in_args[0] == "clear")
			{
				if (in_args.size() == 1)
				{
					cout << "Do you want to clear all the datas? " << endl
						 << "They will disappear for a long time (really long!) (Y/N, case sensitive) " << flush;
					getline(cin, tmp_cmd);

					if (tmp_cmd == "Y")
					{
						for (int i = 1; i <= cfg.ids; i++)
						{
							ss.str("");
							ss << data_dirname << slh << i << ".";
							
							remove((ss.str() + "in").c_str());
							remove((ss.str() + "out").c_str());
						}

						cfg.ids = 0;
						rewrite_cfg(data_dirname + slh + "cfg", cfg);

						cout << "All datas cleared." << endl;
					}
					else if (tmp_cmd == "N")
						cout << "Well...let's continue." << endl;
					else
						cout << "Invalid input" << endl;
				}
				else
					cout << "Error: Too many arguments for command \"clear\"" << endl;
			}
			else if (in_args[0] == "test")
			{
				if (in_args.size() == 1)
				{
					bool is_unac;
					int unacs = 0;

					tmp_cmd = file_name;
					while (tmp_cmd.size() > 0 && tmp_cmd[tmp_cmd.size()-1] != '.')
						tmp_cmd.pop_back();
					tmp_cmd += "exe";

				//	cerr << "Trying to test \"" << tmp_cmd << "\"" << endl;

					for (int i = 1; i <= cfg.ids; i++)
					{
						ss.str("");
						ss << data_dirname << slh << i << ".";
						tmp_dname = ss.str();

						new_file(tmp_dname + "out_t");

						execute_prog(tmp_cmd, tmp_dname + "in", tmp_dname + "out_t");

						is_unac = false;
						try {
							cmp_outs(tmp_dname + "out", tmp_dname + "out_t");
						}
						catch (exception& e)
						{
							cout << "Unaccepted on Test #" << i << ": " << e.what() << endl;
							is_unac = true, unacs++;
						}

						if (!is_unac)
						{
							cout << "Accepted on Test #" << i << endl;
						}

						remove((tmp_dname + "out_t").c_str());
						cout << "Tested " << cfg.ids << " tests. " << unacs << " unaccepted." << endl;
					}
				}
				else
					cout << "Error: Too many arguments for command \"test\"" << endl;
			}
			else if (in_args[0] == "help")
			{
				if (in_args.size() == 1)
				{
					cout << cmd_help << endl;
				}
				else
					cout << "Error: Too many arguments for command \"help\"" << endl;
			}
			else
				cout << "Error: Unknown command." << endl;
		}
	}

	return 0;
}