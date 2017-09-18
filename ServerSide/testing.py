import threading
import OpenValveScript
import sys
import time


try:
	processThread = threading.Thread(target=OpenValveScript.main)
	processThread.daemon = True
	processThread.start()
	#OpenValveScript.main()
	
	while True:
		print("huh")
		time.sleep(5)
		
	
except KeyboardInterrupt:
	sys.exit()


