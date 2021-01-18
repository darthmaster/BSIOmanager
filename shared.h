/* 
 *  shared.h
 *
 *  Created on: 23.09.2020.
 *      Author: ELNA Team
 *
 *  Примечание. НЕ ЗАБУДЬ ДОБАВИТЬ БИБЛИОТЕКУ librt
 */

#ifndef __SHARED_H__
#define __SHARED_H__

#define IO_SHARED_MEMORY_SIZE	(sizeof(shared_t) * 1)	/* Shared memory size */
#define IO_SHARED_MEMORY_NAME	("/bs1_I")				/* Name of shared memory file (name должно иметь в начале косую черту (/)
															и больше не содержать их внутри имени) (/dev/shm/IO_SHARED_REGION) */


/* Достоверность сигнала */
typedef struct {

	unsigned char opc;
	unsigned char valid;
} quality_t;

/* Трассировка входов/выходов (для чего, пока не ясно) */
typedef struct {

	unsigned short count;		/* счетчик пакетов */
	unsigned char  source;		/* источник/приемник пакетов */
	unsigned char  dummy;		/* бесполезный байт для выравнивания */
} trace_t;

/* Статус модуля или устройства */
typedef struct {

    unsigned char  state;		/* код состояния модуля */
    unsigned char  color;		/* код цвета для отображения */
//	unsigned char  dummy[2];	/* бесполезный байт для выравнивания */
	unsigned short type;		/* тип модуля */
	unsigned char  func;		/* функция модуля */
} device_t;

/* Базовые типы ввода или вывода модуля */
typedef struct {
	struct {

		unsigned char type, count;
	} input, output;
} base_IO_t;

/* Дискретный ввод */
typedef struct {

    quality_t      sign;
	unsigned short data[4];
    unsigned char  dummy[2];
} discrete_in_t;

/* Аналоговый ввод */
typedef struct {

    quality_t    sign;
	signed short data[8];
} analog_in_t;

/* Дискретный вывод */
typedef struct {

    quality_t      sign;
	unsigned short data;
//	unsigned char  dummy[2];
} discrete_out_t;

/* Аналоговый вывод */
typedef struct {

    quality_t     sign;
    signed short  data[4];
	unsigned char dummy[2];
} analog_out_t;

/* структура объекта разделяемой памяти (для одного устройства) */
typedef struct {

	/* Структура ВВОДА БС для УСО-модулей */
	struct {
		unsigned short size;
		device_t       device;
		base_IO_t      base_IO;
		trace_t	       trace;
		discrete_in_t  DI;
		analog_in_t    AI;
		trace_t	       execute_trace;
		discrete_out_t DO;
		analog_out_t   AO;
		trace_t        copy_trace;
	} BS_input[16];

	/* Структура ВЫВОДА БС для УСО-модулей */
	struct {
        unsigned short size;
        trace_t        trace;		/* Трассировка задания вывода */
        discrete_out_t DO;
        analog_out_t   AO;
        trace_t        man_trace;	/* Трассировка ручного задания вывода */
        discrete_out_t DO_MAN;		/* Ручное задание дискретного вывода */
        analog_out_t   AO_MAN;		/* Ручное задание аналогового вывода */
    } BS_output[16];
} shared_region_t;


class SharedMem {

	public:
		SharedMem(const char*);

        virtual          ~SharedMem();
		shared_region_t* region;
        void             delmemory(void);

	private:
        int      descriptor;
		unsigned size;
        char     name[32];
        int      connect(int, off_t);
        void     disconnect(int, off_t);
};

/*Пример
	//Запись
 	try {
		SharedMem shared("/shmemory");
		shared.region->value = 1234567;
	} 
	catch(int i) {
		if(i == 1) printf("ERROR: Can't connect to shared memory object.\n");
		else if(i == 2) printf("ERROR: Can't connect to shared memory object.\n");
	}

	//Чтение
	SharedMem shared("/shmemory");
	printf("[%d]\n", shared.region->value);
*/

#endif	/* __SHARED_H__ */
