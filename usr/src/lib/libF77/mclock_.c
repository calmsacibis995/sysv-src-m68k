/*	@(#)mclock_.c	2.1 	*/
long int  mclock_()
  {
  int  buf[6];
  times(buf);
  return(buf[0]+buf[2]+buf[3]);
  }
