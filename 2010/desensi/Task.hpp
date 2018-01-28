/*
 * Task.hpp
 *
 * \date 14/mag/2010
 * \author Daniele De Sensi (desensi@cli.di.unipi.it)
 *
 * This file contains the definition of the task passed by the stages of the pipeline.
 */

#ifndef TASK_HPP_
#define TASK_HPP_
#include "Flow.hpp"
#include "myList.hpp"
#include <allocator.hpp>

/**
 * The task generated by the first stage of the pipeline.
 */
class Task{
private:
	int numWorkers;	///<Number of workers of the pipeline.
	myList<hashElement*> **flowsToAdd,///< A list of flows to add.
		*flowsToExport;///< A list of flows to export.
	bool eof, ///< True if the eof of a .pcap file is arrived.
		readTimeoutExpired; ///< True if the readTimeout is expired on the pcap socket.
	ff::ff_allocator *ffalloc; ///< The fastflow's memory allocator.
public:
	/**
	 * Constructor of the task.
	 * \param numWorkers Number of workers of the pipeline.
	 * \param ffalloc A pointer to the fastflow's memory allocator.
	 */
	inline Task(int numWorkers, ff::ff_allocator *ffalloc):numWorkers(numWorkers),eof(false),readTimeoutExpired(false),ffalloc(ffalloc){
		flowsToAdd=(myList<hashElement*>**) ffalloc->malloc(sizeof(myList<hashElement*>)*numWorkers);
		for(int i=0; i<numWorkers; i++){
			flowsToAdd[i]=(myList<hashElement*>*)ffalloc->malloc(sizeof(myList<hashElement*>));
			flowsToAdd[i]->init(ffalloc);
		}
		flowsToExport=(myList<hashElement*>*)ffalloc->malloc(sizeof(myList<hashElement*>));
		flowsToExport->init();
	}

	/**
	 * Initializes a task.
	 * \param numWorkers Number of workers of the pipeline.
	 * \param ffalloc A pointer to the fastflow's memory allocator.
	 */
	inline void init(int nw, ff::ff_allocator *alloc){
		numWorkers=nw; eof=readTimeoutExpired=false; ffalloc=alloc;
		flowsToAdd=(myList<hashElement*>**) ffalloc->malloc(sizeof(myList<hashElement*>)*numWorkers);
		for(int i=0; i<numWorkers; i++){
			flowsToAdd[i]=(myList<hashElement*>*)ffalloc->malloc(sizeof(myList<hashElement*>));
			flowsToAdd[i]->init(ffalloc);
		}
		flowsToExport=(myList<hashElement*>*)ffalloc->malloc(sizeof(myList<hashElement*>));
		flowsToExport->init();
	}

	/**
	 * Denstructor of the task.
	 */
	inline ~Task(){
		if(flowsToExport!=NULL) ffalloc->free(flowsToExport);
		if(flowsToAdd!=NULL){
			for(int i=0; i<numWorkers; i++)
				ffalloc->free(flowsToAdd[i]);
		}
	}

	/**
	 * Adds an hashElement to the list of flows to export.
	 * \param h The hashElement.
	 */
	inline void addFlowToExport(hashElement* h){
		if(h!=NULL)
			flowsToExport->push(h);
	}

	/**
	 * Returns a pointer to the list of flows to export.
	 * \return A pointer to the list of flows to export.
	 */
	inline myList<hashElement*>* getFlowsToExport(){
		return flowsToExport;
	}

	/**
	 * Returns a pointer to the list of the flows to add.
	 * \return A pointer to the list of the flows to add.
	 */
	inline myList<hashElement*>* getFlowsToAdd(int i){
		return flowsToAdd[i];
	}

	/**
	 * Adds the hashElement h for the i-th worker.
	 * \param h The hashElement to add.
	 * \param i The worker that have to add the flow.
	 */
	inline void setFlowToAdd(hashElement* h, int i){
		flowsToAdd[i]->push(h);
	}

	/**
	 * Sets EOF.
	 */

	inline void setEof(){eof=true;}

	/**
	 * Returns true if EOF of a .pcap file is arrived.
	 * \return True if EOF is arrived, otherwise returns false.
	 */
	inline bool isEof(){return eof;}

	/**
	 * Set the flag for the readTimeoutExpired.
	 */
	inline void setReadTimeoutExpired(){readTimeoutExpired=true;}

	/**
	 * Returns true if the read timeout is expired.
	 * \return true if the read timeout is expired, otherwise returns false.
	 */
	inline bool isReadTimeoutExpired(){return readTimeoutExpired;}

	/**
	 * Returns the number of workers.
	 * \return The number of workers.
	 */
	inline int getNumWorkers(){
		return numWorkers;
	}

	/**
	 * Returns the number of elements to add to the hash tables.
	 * \return The number of elements to add to the hash tables.
	 */
	inline int elementsToAddSize(){
		int size=0;
		for(int i=0; i<numWorkers; i++)
			size+=flowsToAdd[i]->size();
		return size;
	}
};

#endif /* TASK_HPP_ */