
 /* case 3 

 writer can't grab lock together / reader can grab 
 lock together / reader can grab lock even there is waiting
 writer, if writer is waiting for other writers to release 
 their locks

 (1) proc1 grab write_lock(120-180)
 (2) proc2 try write_lock(60-120) => it should wait
 (3) proc3 grab read_lock(60-100)
 (4) proc4 try read_lock(60-100) => should wait
 (5) proc1 unlock, (but nobody grab lock)
 (6) proc3 unlock, (then proc2 grab lock)
 (7) proc2 unlock, (then proc4 grab lock)

 */

 ./set_rotation 120 &
 ./case_common 1 W 150 30 4 &
 ./case_common 2 W 90 30 3 &
 ./set_rotation 100 &
 ./case_common 3 R 80 20 6 &
 ./case_common 4 R 80 20 3 &


 /* case 4 

 same as 3 + writer FIFO (not necessary)
 (1) proc1 grab write_lock(0-140)
 (2) proc2 try write_lock(120-380) => it should wait
 (3) proc3 try write_lock(120-380) => it should wait
 (4) proc4 grab read_lock(150-350)
 (5) proc1 release its lock (but nobody grab lock)
 (6) proc4 release its lock => proc2 grab its lock
 (7) proc2 release its lock => proc3 grab its lock

 */

 ./set_rotation 120 &
 ./case_common 1 W 70 70 7 &
 ./case_common 2 W 250 130 5 &
 ./case_common 3 W 250 130 5 &
 ./set_rotation 250 &
 ./case_common 4 R 250 100 8 &

 /* case 5 

 reader should wait if any writer is waiting for other readers to release their locks
 (1) proc1 grab write_lock(0-60), grab read_lock(270-330)
 (2) proc2 try write_lock(50-180) => it should wait
 (3) proc3 try write_lock(120-300) => it should wait
 (4) proc4 try read_lock(120-180) => it should wait
 (5) proc1 release its write lock => proc2 grab the lock
 (6) proc1 release its read lock => nobody grab the lock
 (7) proc2 release its lock => proc3 grab the lock
 (7) proc3 release its lock => proc4 grab the lock

 */

 ./set_rotation 60 &
 ./case_common 1 W 30 30 10 &
 ./set_rotation 300 &
 ./case_common 1 R 300 30 12 &
 ./case_common 2 W 115 65 5 &
 ./case_common 3 W 210 90 5 &
 ./case_common 4 R 150 30 5 &
 ./set_rotation 130 &

 /* case 6 

 reader grab its lock even there is a writer waiting for other writers to release their locks
 (1) proc1 grab write_lock(0-60)
 (2) proc2 try write_lock(50-180) => it should wait
 (3) proc4 acquire read_lock(120-180)
 (4) proc1 release its write lock => nobody grab lock
 (5) proc4 release its lock => proc2 grab its lock

 */

 ./set_rotation 30 &
 ./case_common 1 W 30 30 5 &
 ./set_rotation 150 &
 ./case_common 2 W 115 65 10 &
 ./case_common 4 R 150 30 10 &

 /* case 7 

 range checking

 T 330
 RL 0 60
 <= Locked R

 */

 ./set_rotation 330 &
 ./case_common 1 R 0 60 10 &

 /* case 8 

 T 30
 RL 330 60
 => Locked R

 */

 ./set_rotation 30 &
 ./case_common 1 R 330 60 10 &

 /* case 9 

 TA's policy
 9.
 T 40
 RL 30 30
 => Locked R
 WL 60 10
 RL 50 10
 => Locked R

 */

 ./set_rotation 40 &
 ./case_common 1 R 30 30 10 &
 ./case_common 2 W 60 10 10 &
 ./case_common 3 R 50 10 10 &


