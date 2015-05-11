#ifndef __MEASURING_TRACE_H_
#define __MEASURING_TRACE_H_

#include "vector"

#include "service_functions.h"

#include "shared/common_vmmkar.h"
// #include "making_decision.h"
// #include "processing_data.h"

/************************************************************************/
/* 
//  [9/18/2010 ik]
		
		Модуль измерения времени:
		- сохраняет в вектор, полученную трасс от драйвера
		- преобразует значения отсчётов в длительность
		- сохраняет матрицу в файл
*/
/************************************************************************/

namespace measuring_trace
{
	/* Получение от драйвера одной трассы с заданными характеристиками */
	bool get_one_trace(IN service_functions :: ServiceManager &csmgr, IN HANDLE hNeither, IN TaskForMeasurement & Task, OUT std :: vector <TimeInterval> &timeIntervals );

	/* Преобразует значения счётчика тактов в длительность выполнения трассы инструкций  (unsigned __int64) */	
	void convert_counts_to_duration(IN std :: vector <TimeInterval> &timeIntervals, OUT std :: vector <unsigned __int64> & traceDuration);

	/* Преобразует значения счётчика тактов в длительность выполнения трассы инструкций (TimeIntervalRes) */
	void convert_counts_to_duration(IN std :: vector <TimeInterval> &timeIntervals, OUT std :: vector <TimeIntervalRes> & traceDuration);

	/* Возвращает путь к временному файлу с учётом расширения, создаёт папку */
	bool set_file_name(TaskForMeasurement &Task, PTCHAR fileTsc, TCHAR ext[]);

	/* Возвращает путь к временному файлу без учёта расширения, создаёт папку */
	bool set_file_name(TaskForMeasurement &Task, PTCHAR fileTsc);

	bool get_one_matrix(service_functions :: ServiceManager &csmgr, HANDLE hNeither, TaskForMeasurement Task, bool & bVirt);


	//////////////////////////////////////////////////////////////////////////


	/* Сохраняет матрицу измерений в файл (unsigned __int64) */
	void save_matrix( TaskForMeasurement &Task, std :: vector < std :: vector<unsigned __int64> > &matrixDuration );

	/* сохраняет матрицу в csv файл */
	void save_matrix_to_csv( TaskForMeasurement &Task, std :: vector < std :: vector<unsigned __int64> > &matrixDuration );
	
	/* сохраняет матрицу в csv файл */
	void save_matrix_to_csv( TaskForMeasurement &Task, std :: vector < std :: vector<unsigned __int64> > &matrixDuration, PTCHAR fileName);
	
	/* сохраняет матрицу в бинарном виде */
	void save_matrix_to_bin( TaskForMeasurement &Task, std :: vector < std :: vector<unsigned __int64> > &matrixDuration, PTCHAR fileName);
	//////////////////////////////////////////////////////////////////////////

	/* Сохраняет измеренные данные в файл (TimeIntervalRes) */
	void save_matrix( TaskForMeasurement &Task, std :: vector < std :: vector<TimeIntervalRes> > &matrixDuration );

	/* Сохраняет столбец времени измерений в файл (TimeIntervalRes) */
	void save_column( TaskForMeasurement &Task, std :: vector < std :: vector<TimeIntervalRes> > &matrixDuration );

	/* чтение матрицы из файла*/
	void read_matrix(PTCHAR fileName, std :: vector < std :: vector<unsigned __int64> > &matrixDuration);
}

#endif // __MEASURING_TRACE_H_