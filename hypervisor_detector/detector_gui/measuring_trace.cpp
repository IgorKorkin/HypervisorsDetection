
#include "stdafx.h"
#include "measuring_trace.h"

namespace measuring_trace
{
	void alloc_buf(IN TaskForMeasurement & taskMeasurement, OUT std :: vector <TimeInterval> & timeIntervals )
	{
		timeIntervals.clear();

		unsigned int entry_counts = 0;

		switch (taskMeasurement.method_type)
		{
		case BlockOfCPUID:
		case CPUIDwithDisabledCache:
		case BlockOfVMXON:
		case VMXONwithDisabledCache:
			entry_counts = taskMeasurement.num_measurements;
			break;

		case WalkThroughMemory:
			entry_counts = (unsigned int) ((MAX_PHYS_MEM_ADDR - taskMeasurement.start_high_phys_addr) / taskMeasurement.phys_mem_offset);			
			break;

		default:{}
		}

		if (timeIntervals.size() < entry_counts)
			{   timeIntervals.resize(entry_counts);   }
	}

	bool get_one_trace(IN service_functions :: ServiceManager &csmgr, IN HANDLE hNeither, IN TaskForMeasurement & Task, OUT std :: vector <TimeInterval> &timeIntervals )
	{
		alloc_buf(Task, timeIntervals);
		
		return csmgr.send_ctrl_code(hNeither, Task.ctrl_code, &Task, sizeof(TaskForMeasurement), timeIntervals.data(), timeIntervals.size() * sizeof(TimeInterval), NULL);
	}
	
	/* (TimeIntervalRes) */
	void convert_counts_to_duration(IN std :: vector <TimeInterval> &timeIntervals, OUT std :: vector <TimeIntervalRes> & traceDuration)
	{
		std :: vector <TimeInterval> :: iterator i;
		TimeIntervalRes elapsed_cycles;
		for ( i = timeIntervals.begin() ; i != timeIntervals.end() ; i++ )
		{
			elapsed_cycles.trace_duration = 
				(((unsigned __int64)i->cycles_high_finish << 32) | i->cycles_low_finish)
				- 
				(((unsigned __int64)i->cycles_high_start << 32) | i->cycles_low_start);

			elapsed_cycles.low_phys_addr = i->low_phys_addr;
			elapsed_cycles.high_phys_addr = i->high_phys_addr;
			elapsed_cycles.valid_address = i->valid_address;
			elapsed_cycles.nt_status = i->nt_status;

			traceDuration.push_back(elapsed_cycles);
		}
	}

	/* (unsigned __int64) */
	void convert_counts_to_duration(IN std :: vector <TimeInterval> &timeIntervals, OUT std :: vector <unsigned __int64> & traceDuration)
	{
		std :: vector <TimeInterval> :: iterator i;
		unsigned int elapsed_cycles = 0;
		for ( i = timeIntervals.begin() ; i != timeIntervals.end() ; i++ )
		{
			elapsed_cycles = 
				(((unsigned __int64)i->cycles_high_finish << 32) | i->cycles_low_finish)
				- 
				(((unsigned __int64)i->cycles_high_start << 32) | i->cycles_low_start);
			traceDuration.push_back(elapsed_cycles);
		}
	}

	/* Возвращает путь к временному файлу с учётом расширения, создаёт папку */
	bool set_file_name(TaskForMeasurement &Task, PTCHAR fileTsc, TCHAR ext[])
	{
		bool b_res = false;
		int i_folder_traces = 0;
		const int sz_folder_traces = MAX_PATH;
		TCHAR folder_traces[sz_folder_traces] = {0};

		i_folder_traces += _stprintf_s(folder_traces + i_folder_traces, sz_folder_traces - i_folder_traces, TEXT("%s "), g_MethodDetectionName[Task.method_type] );

		i_folder_traces += _stprintf_s(folder_traces + i_folder_traces, sz_folder_traces - i_folder_traces, 
			TEXT("TraceLen=%d NumMeasur=%d Affinity=0x%x"), Task.trace_length, Task.num_measurements, Task.affinity );		

		if (CreateDirectory(folder_traces, NULL) || (ERROR_ALREADY_EXISTS == GetLastError()) )
		{
			SYSTEMTIME system_time;
			GetLocalTime(&system_time);

			_stprintf_s(fileTsc, MAX_PATH, TEXT("%s\\trace %02d.%02d.%d %02d;%02d;%02d;%d_%d.%s"), folder_traces, system_time.wDay, system_time.wMonth, system_time.wYear, 
				system_time.wHour, system_time.wMinute, system_time.wSecond, system_time.wMilliseconds, rand()%100, ext );
			b_res = true;
		}
		return b_res;
	}

	/* Возвращает путь к временному файлу без учёта расширения, создаёт папку */
	bool set_file_name(TaskForMeasurement &Task, PTCHAR fileTsc)
	{
		bool b_res = false;
		int i_folder_traces = 0;
		const int sz_folder_traces = MAX_PATH;
		TCHAR folder_traces[sz_folder_traces] = {0};

		i_folder_traces += _stprintf_s(folder_traces + i_folder_traces, sz_folder_traces - i_folder_traces, TEXT("%s "), g_MethodDetectionName[Task.method_type] );

		i_folder_traces += _stprintf_s(folder_traces + i_folder_traces, sz_folder_traces - i_folder_traces, 
			TEXT("TraceLen=%d NumMeasur=%d Affinity=0x%x"), Task.trace_length, Task.num_measurements, Task.affinity );		

		if (CreateDirectory(folder_traces, NULL) || (ERROR_ALREADY_EXISTS == GetLastError()) )
		{
			SYSTEMTIME system_time;
			GetLocalTime(&system_time);

			_stprintf_s(fileTsc, MAX_PATH, TEXT("%s\\trace %02d.%02d.%d %02d;%02d;%02d;%d_%d"), folder_traces, system_time.wDay, system_time.wMonth, system_time.wYear, 
				system_time.wHour, system_time.wMinute, system_time.wSecond, system_time.wMilliseconds, rand()%100 );
			b_res = true;
		}
		return b_res;
	}

	//////////////////////////////////////////////////////////////////////////
	
	/* Сохраняет матрицу измерений в файл (unsigned __int64) */
	void save_matrix( TaskForMeasurement &Task, std :: vector < std :: vector<unsigned __int64> > &matrixDuration )
	{	
		TCHAR file_tsc[MAX_PATH] = {0}, file_tsc_csv[MAX_PATH] = {0}, file_tsc_bin[MAX_PATH] = {0};
		if (set_file_name(Task, file_tsc))
		{
			if (_stprintf_s(file_tsc_csv, TEXT("%s.csv"), file_tsc) > 0 )
				{   save_matrix_to_csv(Task, matrixDuration, file_tsc_csv);   }

		}
	}

	void save_matrix_to_csv( TaskForMeasurement &Task, std :: vector < std :: vector<unsigned __int64> > &matrixDuration, PTCHAR fileName)
	{			
		int cur_buf = 0;
		const DWORD sz_buf = MAX_PATH;
		CHAR buf[sz_buf] = {0};

		HANDLE file_wr = NULL;
		if ((HANDLE)INVALID_HANDLE_VALUE != (file_wr = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) )
		{
			int col = matrixDuration.size();
			int row = matrixDuration[0].size();

			for (int j = 0; j < row ; j++)
			{
				for (int i = 0; i < col ; i++)
				{
					cur_buf = sprintf_s(buf, sz_buf, ("%d;"), matrixDuration[ i ][ j ] );
					
					if(INVALID_SET_FILE_POINTER != SetFilePointer(file_wr, 0, NULL, FILE_END) )
					{
						DWORD number_of_bytes_written = 0;
						if (WriteFile(file_wr, buf, cur_buf, &number_of_bytes_written, NULL) &&
							(number_of_bytes_written == cur_buf))
							{   }
					}
				}
				cur_buf = sprintf_s(buf, sz_buf, ("\n") );

				if(INVALID_SET_FILE_POINTER != SetFilePointer(file_wr, 0, NULL, FILE_END) )
				{
					DWORD number_of_bytes_written = 0;
					if (WriteFile(file_wr, buf, cur_buf, &number_of_bytes_written, NULL) &&
						(number_of_bytes_written == cur_buf))
					{   }
				}
			}
			if (file_wr)   {   CloseHandle(file_wr);   }
		}
	}

	void save_matrix( TaskForMeasurement &Task, std :: vector < std :: vector<TimeIntervalRes> > &matrixDuration )
	{
		unsigned int elapsed_cycles = 0;
		int cur_buf = 0;
		const DWORD sz_buf = MAX_PATH * 2;
		CHAR buf[sz_buf] = {0};

		TCHAR file_tsc[MAX_PATH] = {0};
		if (set_file_name(Task, file_tsc, TEXT("csv")))
		{
			HANDLE file_wr = NULL;
			if ((HANDLE)INVALID_HANDLE_VALUE != (file_wr = CreateFile(file_tsc, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) )
			{
				int col = matrixDuration.size();
				int row = matrixDuration[0].size();

				cur_buf = sprintf_s(buf, sz_buf, ("low_phys_addr;high_phys_addr;trace_duration_timeres;valid_address;nt_status;;\n"));

				if(INVALID_SET_FILE_POINTER != SetFilePointer(file_wr, 0, NULL, FILE_END) )
				{
					DWORD number_of_bytes_written = 0;
					if (WriteFile(file_wr, buf, cur_buf, &number_of_bytes_written, NULL) &&
						(number_of_bytes_written == cur_buf))
					{   }
				}

				for (int j = 0; j < row ; j++)
				{
					for (int i = 0; i < col ; i++)
					{
						cur_buf = sprintf_s(buf, sz_buf, ("%016I64x;%016I64x;%I64d;%i;%08X;;"), 
							matrixDuration[ i ][ j ].low_phys_addr, matrixDuration[ i ][ j ].high_phys_addr, matrixDuration[ i ][ j ].trace_duration, 
							matrixDuration[ i ][ j ].valid_address, matrixDuration[ i ][ j ].nt_status);

						if(INVALID_SET_FILE_POINTER != SetFilePointer(file_wr, 0, NULL, FILE_END) )
						{
							DWORD number_of_bytes_written = 0;
							if (WriteFile(file_wr, buf, cur_buf, &number_of_bytes_written, NULL) &&
								(number_of_bytes_written == cur_buf))
							{   }
						}
					}
					
					cur_buf = sprintf_s(buf, sz_buf, ("\n") );
					if(INVALID_SET_FILE_POINTER != SetFilePointer(file_wr, 0, NULL, FILE_END) )
					{
						DWORD number_of_bytes_written = 0;
						if (WriteFile(file_wr, buf, cur_buf, &number_of_bytes_written, NULL) &&
							(number_of_bytes_written == cur_buf))
						{   }
					}
				}
				if (file_wr)   {   CloseHandle(file_wr);   }
			}
		}
	}

	/* Сохраняет столбец времени измерений в файл (TimeIntervalRes) */
	void save_column( TaskForMeasurement &Task, std :: vector < std :: vector<TimeIntervalRes> > &matrixDuration )
	{
		unsigned int elapsed_cycles = 0;
		int cur_buf = 0;
		const DWORD sz_buf = MAX_PATH * 2;
		CHAR buf[sz_buf] = {0};

		TCHAR file_tsc[MAX_PATH] = {0};
		if (set_file_name(Task, file_tsc, TEXT("csv")))
		{
			HANDLE file_wr = NULL;
			if ((HANDLE)INVALID_HANDLE_VALUE != (file_wr = CreateFile(file_tsc, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) )
			{
				int col = matrixDuration.size();
				int row = matrixDuration[0].size();

				for (int j = 0; j < row ; j++)
				{
					for (int i = 0; i < col ; i++)
					{
						cur_buf = sprintf_s(buf, sz_buf, ("%I64d;"), 
							matrixDuration[ i ][ j ].trace_duration);

						if(INVALID_SET_FILE_POINTER != SetFilePointer(file_wr, 0, NULL, FILE_END) )
						{
							DWORD number_of_bytes_written = 0;
							if (WriteFile(file_wr, buf, cur_buf, &number_of_bytes_written, NULL) &&
								(number_of_bytes_written == cur_buf))
							{   }
						}

					}
					cur_buf = sprintf_s(buf, sz_buf, ("\n") );
					if(INVALID_SET_FILE_POINTER != SetFilePointer(file_wr, 0, NULL, FILE_END) )
					{
						DWORD number_of_bytes_written = 0;
						if (WriteFile(file_wr, buf, cur_buf, &number_of_bytes_written, NULL) &&
							(number_of_bytes_written == cur_buf))
						{   }
					}
				}
				if (file_wr)   {   CloseHandle(file_wr);   }
			}
		}
	}

	bool get_one_matrix(service_functions :: ServiceManager &csmgr, HANDLE hNeither, TaskForMeasurement Task, bool & bVirt)
	{
		bool b_res = false;
		std :: vector <TimeInterval> time_intervals;
		switch (Task.method_type)
		{
		case BlockOfCPUID:
		case BlockOfVMXON:
		case CPUIDwithDisabledCache:
		case VMXONwithDisabledCache:
			{
				std :: vector<unsigned __int64> trace_duration__int64;
				std :: vector < std :: vector<unsigned __int64> > matrix_duration;
				for (unsigned int i = 0 ; i < Task.num_repetitions; i++ )
				{
					if (measuring_trace :: get_one_trace(csmgr, hNeither, Task, time_intervals))
					{
						measuring_trace :: convert_counts_to_duration(time_intervals, trace_duration__int64);
						matrix_duration.push_back(trace_duration__int64);
						trace_duration__int64.clear();
					}			
					Sleep(Task.delay_between_rep);
				}
				measuring_trace :: save_matrix(Task, matrix_duration);
				break;
			}
		case WalkThroughMemory:
			{
				__debugbreak();
				std :: vector<TimeIntervalRes> trace_duration_timeres;
				std :: vector < std :: vector<TimeIntervalRes> > matrix_duration;
				for (unsigned int i = 0 ; i < Task.num_repetitions; i++ )
				{
					if (measuring_trace :: get_one_trace(csmgr, hNeither, Task, time_intervals))
					{
						measuring_trace :: convert_counts_to_duration(time_intervals, trace_duration_timeres);
						matrix_duration.push_back(trace_duration_timeres);
						trace_duration_timeres.clear();
					}
					Sleep(Task.delay_between_rep);
				}
				measuring_trace :: save_matrix(Task, matrix_duration);
				measuring_trace :: save_column(Task, matrix_duration);
				break;
			}
		default:{}
		}
		return b_res;
	}

}