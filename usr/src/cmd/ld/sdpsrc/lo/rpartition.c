/*	@(#)rpartition.c	2.1		*/
/*
 * static char ID_reprtc[] = "@(#) rpartition.c: 1.1 11/23/82";
 */

repartition()
{

#if DPARTRESET != -1
	register int index1, index2, limit1, limit2;
	register usage;

/*
 * Redefine the size of the partitions
 */

	usage = partusage[num_partitions];

	if( dflag > 10 ) {
		fprintf( stderr, "\nrepartition: %d :", num_partitions );
		for( index1 = 0; index1 < num_partitions; index1++ )
			fprintf( stderr, " (%5d %5d %5d %5d %5d)",
				partitions[index1], num_linked[index1], lru[index1], mru[index1], partusage[index1] );
		}
		
	index2 = num_partitions - 1;
	partitions[index2] = NUMBERFRAMES;
	partusage[index2] = 0;
	for( index1 = 0; index1 < index2; index1++ ) {
		partitions[index1] = (partusage[index1] * NUMBERFRAMES)/usage;
		if( partitions[index1] == 0 )
			partitions[index1] = 1;
		partusage[index1] = 0;
		partitions[index2] -= partitions[index1];
		}
	partusage[num_partitions] = 0;

	if( dflag > 10 ) {
		fprintf( stderr, "\n\t\t" );
		for( index1 = 0; index1 < num_partitions; index1++ )
			fprintf( stderr, " (%5d %5d %5d %5d %5d)",
				partitions[index1], num_linked[index1], lru[index1], mru[index1], partusage[index1] );
		}

/*
 * Move the page frames into the new partitons
 */

	if( dflag > 12 ) {
		fprintf( stderr, "\n" );
		for( index1 = 0; index1 < NUMBERFRAMES; index1++ )
			fprintf( stderr, " %2d", index1 );
		fprintf( stderr, "\n" );
		for( index1 = 0; index1 < NUMBERFRAMES; index1++ )
			fprintf( stderr, " %2d", manager[index1].partition );
		fprintf( stderr, "\n" );
		for( index1 = 0; index1 < NUMBERFRAMES; index1++ )
			fprintf( stderr, " %2d", manager[index1].backward );
		fprintf( stderr, "\n" );
		for( index1 = 0; index1 < NUMBERFRAMES; index1++ )
			fprintf( stderr, " %2d", manager[index1].forward );
		}

	limit1 = 0;
	for( index1 = 0; index1 < num_partitions; index1++ ) {

		limit2 = limit1 + partitions[index1];

		for( index2 = limit1; index2 < limit2; index2++ ) {
			manager[index2].partition = index1;
			if( manager[index2].forward != INVALID )
				break;
			}

		if( index2 == limit2 ) {
			lru[index1] = mru[index1] = NOFRAMES;
			num_linked[index1] = 0;
			continue;
			}
		
		lru[index1] = mru[index1] = index2;
		num_linked[index1] = 1;

		for( index2++; index2 < limit2; index2++ ) {
			manager[index2].partition = index1;
			if( manager[index2].forward == INVALID )
				continue;
			num_linked[index1]++;
			manager[mru[index1]].forward = index2;
			manager[index2].backward = mru[index1];
			mru[index1] = index2;
			}

		manager[lru[index1]].backward = LAMDA;
		manager[mru[index1]].forward = LAMDA;

		limit1 = limit2;

		}

	if( dflag > 12 ) {
		fprintf( stderr, "\n\n" );
		for( index1 = 0; index1 < NUMBERFRAMES; index1++ )
			fprintf( stderr, " %2d", manager[index1].partition );
		fprintf( stderr, "\n" );
		for( index1 = 0; index1 < NUMBERFRAMES; index1++ )
			fprintf( stderr, " %2d", manager[index1].backward );
		fprintf( stderr, "\n" );
		for( index1 = 0; index1 < NUMBERFRAMES; index1++ )
			fprintf( stderr, " %2d", manager[index1].forward );
		}

#endif
}
