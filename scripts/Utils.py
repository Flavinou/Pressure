import requests
import sys
import time

from fake_useragent import UserAgent

def DownloadFile(url, filepath):
	with open(filepath, "wb") as file:
			ua = UserAgent()
			headers = {"User-Agent": ua.random}
			response = requests.get(url, headers=headers, stream=True)
			response.raise_for_status()
			total = response.headers.get("content-length")
			if total is None:
				file.write(response.content)
			else:
				downloaded = 0
				total = int(total)
				startTime = time.time()
				for data in response.iter_content(chunk_size=max(int(total / 1000), 1024 * 1024)):
					downloaded += len(data)
					file.write(data)
					done = int(50 * downloaded / total)
					percentage = (downloaded / total) * 100
					elapsedTime = time.time() - startTime
					avgKBPerSecond = (downloaded / 1024) / elapsedTime
					avgSpeedString = "{:.2f} KB/s".format(avgKBPerSecond)
					if avgKBPerSecond > 1024:
						avgMBPerSecond = avgKBPerSecond / 1024
						avgSpeedString = "{:.2f} MB/s".format(avgMBPerSecond)
					sys.stdout.write(
						"\r[{}{}] {:.2f}% ({})     ".format("█" * done, "." * (50 - done), percentage, avgSpeedString)
					)
					sys.stdout.flush()
	sys.stdout.write("\n")

def YesOrNo():
	while True:
		response = input("Enter 'y' for yes or 'n' for no: ").strip().lower()
		if response == 'y':
			return True
		elif response == 'n':
			return False
		else:
			print("Invalid input. Please enter 'y' or 'n'.")
