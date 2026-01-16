#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <time.h>

typedef struct List {
	PROCESS_INFORMATION pi;
	char name[100];
	int status;
	struct List* next;
} List;

List* head = NULL;
char **allPath = NULL;
int sumPath = 0;
HANDLE fProcess = NULL;

BOOL WINAPI CtrlHandler(DWORD ctrlType) {
	if (ctrlType == CTRL_C_EVENT) {
		if (fProcess != NULL) {
			printf("Dang tat tien trinh...\n");
			TerminateProcess(fProcess, 1);
			fProcess = NULL;
		}
		return TRUE;
	}
	return FALSE;
}

int fileExists(char *path) {
	DWORD dwAttri = GetFileAttributes(path);
	return (dwAttri != INVALID_FILE_ATTRIBUTES && !(dwAttri & FILE_ATTRIBUTE_DIRECTORY));
}

char* findAbsolutePath(char* fileName) {
	static char fullPath[MAX_PATH]; 
	
	strcpy(fullPath, fileName);
	if (fileExists(fullPath)) {
		return fullPath;
	}
	for (int i = 0; i < sumPath; i++) {
		sprintf(fullPath, "%s\\%s", allPath[i], fileName);
		if (fileExists(fullPath)) {
			return fullPath;
		}
	}
	return NULL;
}

void addProcessToList(PROCESS_INFORMATION pi, char* name) {
	List* tmp = (List*)malloc(sizeof(List));
	tmp->pi = pi;
	strcpy(tmp->name, name);
	tmp->status = 1;
	tmp->next = NULL;
	if (head == NULL) {
		head = tmp;
	} else {
		List* cur = head;
		while (cur->next != NULL) {
			cur = cur->next;
		}
		cur->next = tmp;
	}
}

void cleanupNode(List* node) {
	if (node) {
		CloseHandle(node->pi.hProcess);
		CloseHandle(node->pi.hThread);
		free(node);
	}
}

void killAllProcesses() {
	List* cur = head;
    while (cur != NULL) {
        TerminateProcess(cur->pi.hProcess, 0);
        List* temp = cur;
        cur = cur->next;
        cleanupNode(temp);
    }
}

void removeProcess(int num) {
	List* cur = head;
	List* pre_cur = NULL;
	int cnt = 0;
	while (cur != NULL && cnt < num) {
		cnt++;
		pre_cur = cur;
		cur = cur->next;
	}
	if (cur == NULL) {
		printf("Khong tim thay process thu %d\n", num);
		return;
	}
	printf("Dang xoa tien trinh thu %d...\n", num);
	TerminateProcess(cur->pi.hProcess, 0);
	if (pre_cur == NULL) {
		head = cur->next;
	} else {
		pre_cur->next = cur->next;
	}
	cleanupNode(cur);
	printf("Da xoa thanh cong\n");
}

void stopProcess(int num) {
	List* cur = head;
	int cnt = 0;
	while (cur != NULL && cnt < num) {
		cur = cur->next;
		cnt++;
	}
	if (cur == NULL) {
		printf("Khong tim thay process thu %d\n", num);
		return;
	}
	SuspendThread(cur->pi.hThread);
	cur->status = 0;
	printf("Da tam dung tien trinh\n");
}

void resumeProcess(int num) {
	List* cur = head;
	int cnt = 0;
	while (cur != NULL && cnt < num) {
		cur = cur->next;
		cnt++;
	}
	if (cur == NULL) {
		printf("Khong tim thay tien trinh thu %d\n", num);
		return;
	} else if (cur->status != 0) {
		printf("Tien trinh chua tam dung\n");
		return;
	}
	ResumeThread(cur->pi.hThread);
	cur->status = 1;
	printf("Tien trinh tiep tuc hoat dong\n");	
}

void list() {
	List* cur = head;
	List* pre_cur = NULL;
	int cnt = 0;
	printf("Danh sach tien trinh la\n");
	while (cur != NULL) {
		DWORD result = WaitForSingleObject(cur->pi.hProcess, 0);
		if (result == WAIT_OBJECT_0) {
			List* tmp = cur;
			if (pre_cur == NULL) {
				head = cur->next;
			} else {
				pre_cur->next = cur->next;
			}
			cur = cur->next;
			cleanupNode(tmp);
			continue;
		} else {	
			char status[10];
			if (cur->status == 1) {
				strcpy(status, "Running");
			} else if (cur->status == 0) {
				strcpy(status, "Stopping");
			}
			printf("%d. Process ID: %d Process Name: %s Status: %s\n",cnt ,cur->pi.dwProcessId, cur->name, status);
		}
		pre_cur = cur;
		cur = cur->next;
		cnt++;
	}
}

int chooseProcessFromList() {
	list();
	int choose;
	printf("Ban muon chon tien trinh nao: ");
	scanf("%d", &choose);
	getchar();
	return choose;
}

void createForeProcess(char* cmd) {
    char fullCmd[MAX_PATH];
    char* foundPath = findAbsolutePath(cmd);
	if (foundPath != NULL) {
		strcpy(fullCmd, foundPath);
	} else {
		sprintf(fullCmd, cmd);
	}
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    BOOL check = CreateProcess(NULL, fullCmd, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
    if (check == TRUE) {
    	printf("Tien trinh da duoc tao thanh cong \nAn Ctrl + C de tat tien trinh\n");
    	fProcess = pi.hProcess;
	    WaitForSingleObject(pi.hProcess, INFINITE);
	    CloseHandle(pi.hProcess);
	    CloseHandle(pi.hThread);
	    printf("Da tat tien trinh\n");
	} else {
		printf("Tien trinh khong ton tai\n");
	}
}

PROCESS_INFORMATION createBackProcess(char* cmd) {
    char fullCmd[MAX_PATH];
    char* foundPath = findAbsolutePath(cmd);
	if (foundPath != NULL) {
		strcpy(fullCmd, foundPath);
	} else {
		sprintf(fullCmd, cmd);
	}
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    BOOL check = CreateProcess(NULL, fullCmd, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
    if (check == TRUE) {
    	printf("Tao tien trinh chay song song thanh cong\n");
    	addProcessToList(pi, cmd);
	} else {
		printf("Loi khong tao duoc tien trinh\n");
	}
    return pi;
}

void help() {
   FILE *file = fopen("help.txt", "r"); 
   if (file == NULL) {
       printf("Loi khong thay file help\n");
       return;
   }
   char buffer[100]; 
   while (fgets(buffer, sizeof(buffer), file)) {
       printf("%s", buffer); 
   }
   fclose(file); 
}

void cur_time() {
	time_t current_time;
    time(&current_time);
    struct tm *local_time = localtime(&current_time);
    printf("Ngay/Thang/Nam: %02d/%02d/%04d\n", 
           local_time->tm_mday, 
           local_time->tm_mon + 1, 
           local_time->tm_year + 1900);
    printf("Thoi gian: %02d:%02d:%02d\n", 
           local_time->tm_hour, 
           local_time->tm_min, 
           local_time->tm_sec);
}

void directory(char *path){
	sprintf(path, "%s\\*", path);
    WIN32_FIND_DATA fd;
    HANDLE FFF = FindFirstFile(path,&fd);
    printf("      Time       -- Name\n", "Time");
    do{
    	SYSTEMTIME systemTime;
	    if (FileTimeToSystemTime(&fd.ftCreationTime, &systemTime)) {
	        printf("%04d/%02d/%02d %02d:%02d -- ", systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute);
	    } else {
	        printf("Khong chuyen duoc filetime thanh systemtime");
	    }
        printf("%s\n" ,fd.cFileName);
    }while(FindNextFile(FFF,&fd) );
    FindClose(FFF);
}

int loadPath(char** allPath) {
	FILE *file = fopen("path.txt", "r");
	if (file == NULL) {
		printf("Khong tim thay file chua path\n");
		return 0;
	}
	int cnt = 0;
	char buffer[256];
	while(fgets(buffer, sizeof(buffer), file)) {
		buffer[strcspn(buffer, "\n")] = 0;
		if (strlen(buffer) == 0) continue;
		allPath[cnt] = strdup(buffer);
		cnt++;
	}
	fclose(file);
	return cnt;
}

int addPath() {
	FILE *file = fopen("path.txt", "a");
	if (file == NULL) {
		printf("Khong tim thay file chua path\n");
		return sumPath;
	}
	char buffer[MAX_PATH];
	printf("Nhap path ban muon them vao day: ");
	fgets(buffer, sizeof(buffer), stdin);
	fprintf(file, buffer);
	buffer[strcspn(buffer, "\n")] = 0;
	allPath[sumPath] = strdup(buffer);
	sumPath++;
	fclose(file);
	return sumPath;
}

void rewritePathFile() {
    FILE *file = fopen("path.txt", "w"); 
    if (file == NULL) {
        printf("Loi: Khong the ghi file path.txt\n");
        return;
    }
    
    for (int i = 0; i < sumPath; i++) {
        fprintf(file, "%s\n", allPath[i]);
    }
    
    fclose(file);
}

int removePath() {
    if (sumPath == 0) {
        printf("Danh sach Path dang trong.\n");
        return 0;
    }

    printf("--- DANH SACH DUONG DAN ---\n");
    for (int i = 0; i < sumPath; i++) {
        printf("%d. %s\n", i + 1, allPath[i]); 
    }
    printf("---------------------------\n");

    int choice;
    printf("Nhap so thu tu muon xoa (Nhap 0 de huy): ");
    if (scanf("%d", &choice) == 0) {
        choice = 0; 
    }
    
    int c; while ((c = getchar()) != '\n' && c != EOF);

    if (choice == 0) {
        printf("Da huy thao tac.\n");
        return sumPath;
    }

    if (choice < 1 || choice > sumPath) {
        printf("Loi: So thu tu khong hop le!\n");
        return sumPath;
    }

    int index = choice - 1; 
    
    printf("Dang xoa: %s\n", allPath[index]);
    free(allPath[index]); 

    for (int i = index; i < sumPath - 1; i++) {
        allPath[i] = allPath[i+1];
    }
    
    sumPath--;
    rewritePathFile(); 

    printf("Da cap nhat path.txt thanh cong.\n");
    return sumPath;
}

int main(){
	allPath = (char **)malloc(100 * sizeof(char *));
	sumPath = loadPath(allPath);
	if (SetConsoleCtrlHandler(CtrlHandler, TRUE) == FALSE) {
        printf("Loi: Khong the dang ky Ctrl Handler\n");
        return 1;
    }
	while (1) {
		char currentDir[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, currentDir);
		printf("MyShell ~%s>", currentDir);
	    char command[100];
	    fgets(command, sizeof(command), stdin);
	    command[strlen(command)-1] = '\0';
	    char* token = strtok(command," ");
	    int cnt = 0;
	    char **cmd = (char **)malloc(10 * sizeof(char *));
	    while (token) {
	    	cmd[cnt] = strdup(token);
	    	token = strtok(NULL, " ");
	    	cnt++;
		}
		if (cnt == 2) {
			if (strcmp(cmd[0], "bopen") == 0) {
				createBackProcess(cmd[1]);
			} else if (strcmp(cmd[0], "fopen") == 0) {
				createForeProcess(cmd[1]);
			} else if (strcmp(cmd[0], "cd") == 0) {
				if (!SetCurrentDirectory(cmd[1])) {
					printf("Khong tim thay thu muc");	
				}
			} else {
				printf("Lenh khong ton tai\n");
			}
		} else if (cnt == 1) {
			if (strcmp(cmd[0], "list") == 0) {
				list();
			} else if (strcmp(cmd[0], "exit") == 0) {
				killAllProcesses();
                for (int i = 0; i < cnt; i++) free(cmd[i]);
                free(cmd);
                for (int i = 0; i < sumPath; i++) free(allPath[i]);
                free(allPath);
				break;
			} else if (strcmp(cmd[0], "remove") == 0) {
				int cnt = chooseProcessFromList();
				removeProcess(cnt);
			} else if (strcmp(cmd[0], "stop") == 0) {
				int cnt = chooseProcessFromList();
				stopProcess(cnt);
			} else if (strcmp(cmd[0], "resume") == 0) {
				int cnt = chooseProcessFromList();
				resumeProcess(cnt);
			} else if (strcmp(cmd[0], "help") == 0) {
				help();
			} else if (strcmp(cmd[0], "time") == 0) {
				cur_time();
			} else if (strcmp(cmd[0], "path") == 0) {			
				for (int i = 0; i < sumPath; i++) {
					printf("%s\n", allPath[i]);
				}
			} else if (strcmp(cmd[0], "addpath") == 0) {
				sumPath = addPath();
			} else if (strcmp(cmd[0], "rmpath") == 0) {
				sumPath = removePath();
			} else if (strcmp(cmd[0], "dir") == 0) {
				directory(currentDir);
			} else {
				printf("Lenh khong ton tai\n");
			}
		} else {
			printf("Nhap lenh loi\n");
		}
		for (int i = 0; i < cnt; i++) free(cmd[i]);
		free(cmd);
		printf("\n");
	}	
}
