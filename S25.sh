#!/bin/bash


#This is our record file which will have all the logs of the files backed up
rec="$HOME/s25log.txt"
#This is our root directory
strt="/home/$USER"
FBUP_DIR="$HOME/backup/fbup"
IBUP_DIR="$HOME/backup/ibup"
DBUP_DIR="$HOME/backup/dbup"
ISBUP_DIR="$HOME/backup/isbup"


# STEP 1 - FULL BACKUP with 2-minute delay
frstStepBckup() {

     #Here we are getting the timing
    taime=$(date '+%a %d %b%Y %I:%M:%S %p %Z')
     #If no file exten is passed meaning $#
    # 1. Build find command
    if [ "$#" -eq 0 ]; then
        FILES_TO_BACKUP=$(find "$strt" -type f)  #then back up all files type f from root
    else
        FIND_CMD=(find "$strt" -type f \( )  #this case deals with if instructions are given
        for EXT in "$@"; do  #if there is an ext in $@ --> all arg given
            FIND_CMD+=(-name "*$EXT" -o) #loop over all the extensions (start of functions)
        done
        unset 'FIND_CMD[${#FIND_CMD[@]}-1]'  # Remove trailing -o
        FIND_CMD+=(\)) #close the grouping of name
        FILES_TO_BACKUP=$("${FIND_CMD[@]}") #here we use the find command using array
    fi
     #Here we are finding the number of files in our directory
    FILE_COUNT=$(echo "$FILES_TO_BACKUP" | grep -c '^') #grep counts how many lines match that
     #if the count is zero then
    if [ "$FILE_COUNT" -eq 0 ]; then
 echo "$taime No changes-Incremental backup was not created" >> "$rec"

    else
        #This is the main part of the code
        #List all full back up files
        #Extract the number using cut
        #Sort them
        #Takes the highest one and adds 1
        #Default to 1 is none exist
        INDEX=$(($(ls "$FBUP_DIR" 2>/dev/null | grep -Eo 'fbup-([0-9]+)\.tar' | cut -d'-' -f2 | cut -d'.' -f1 | sort -n | tail -n1) + 1))
        INDEX=${INDEX:-1}
        saving_file="fbup-$INDEX.tar" #this constructs a new tar file name and add index to it

        # Here we are creating tar files
        #c -> create archive f -> file name for archive
        #tar tells to read the list of files from stdin
        # - means to read from a stream input
        echo "$FILES_TO_BACKUP" | tar -cf "$FBUP_DIR/$saving_file" -T - 2>/dev/null
        echo "$taime $saving_file was created" >> "$rec" #adding it to the log file
    fi

    echo "Debugging statement sleeping 120s after full backup..."
    sleep 120
}

perform_incremental_after_full() {
    #we take time using the date formatted version
    taime=$(date '+%a %d %b%Y %I:%M:%S %p %Z')

    # 1. Get the taime of the most recent full backup
     # -t is sorting by time
      #head -n 1 picks the latest file
      #if no file is found then output is empty
    LAST_FB=$(ls -t "$FBUP_DIR"/fbup-*.tar 2>/dev/null | head -n 1)
    if [ -z "$LAST_FB" ]; then #-z checks for the string if its empty or not
        echo "$taime There is no incremental backup: No full backup found" >> "$rec"
        sleep 120
        return
    fi
    FB_TIME=$(date -r "$LAST_FB" +%s) #this gets the time of last modification and changes to the unix timestamp

    # 2. Build the find command
    if [ "$#" -eq 0 ]; then #if no ext passed then update new files
        MATCHED_FILES=$(find "$strt" -type f -newermt "@$FB_TIME")
    else #if commands are passed then build the find function
        FIND_CMD=(find "$strt" -type f \( )
        for EXT in "$@"; do
            FIND_CMD+=(-name "*$EXT" -o)
        done
         #to stop from the last -o from syntax error we do this
        unset 'FIND_CMD[${#FIND_CMD[@]}-1]'
        FIND_CMD+=(\) -newermt "@$FB_TIME") #we group and use newermt to find the lastest
        MATCHED_FILES=$("${FIND_CMD[@]}")
    fi
    #here we are counting the number of files
    FILE_COUNT=$(echo "$MATCHED_FILES" | grep -c '^')
    #if no count has returned then
    if [ "$FILE_COUNT" -eq 0 ]; then
        echo "$taime No changes-Incremental backup was not created" >> "$rec"
    else
        # Here we are building a tar file and logging in file
        INDEX=$(($(ls "$IBUP_DIR" 2>/dev/null | grep -Eo 'ibup-([0-9]+)\.tar' | cut -d'-' -f2 | cut -d'.' -f1 | sort -n | tail -n 1) + 1))
        INDEX=${INDEX:-1} #we cant have it as 0 because it will throw an error
        saving_file="ibup-$INDEX.tar" #creating file name here
        echo "$MATCHED_FILES" | tar -cf "$IBUP_DIR/$saving_file" -T - 2>/dev/null
        echo "$taime $saving_file was created" >> "$rec"
    fi

    echo "Debugging Sleeping 120s after incremental backup..."
    sleep 120 #this sleeps for that long
}
perform_incremental_after_incremental() {
    #here we are getting time
    taime=$(date '+%a %d %b%Y %I:%M:%S %p %Z')

    # 1. Get taime of last incremental (STEP 2) backup
    LAST_IBUP=$(ls -t "$IBUP_DIR"/ibup-*.tar 2>/dev/null | head -n 1)
    if [ -z "$LAST_IBUP" ]; then
        echo "$taime STEP 3 skipped: No previous incremental backup found" >> "$rec"
        sleep 120
        return
    fi
    LAST_IBUP_TIME=$(date -r "$LAST_IBUP" +%s)

    # 2. Build find command with extension filters
    if [ "$#" -eq 0 ]; then
        MATCHED_FILES=$(find "$strt" -type f -newermt "@$LAST_IBUP_TIME")
    else
        FIND_CMD=(find "$strt" -type f \( )
        for EXT in "$@"; do
            FIND_CMD+=(-name "*$EXT" -o)
        done
        unset 'FIND_CMD[${#FIND_CMD[@]}-1]'
        FIND_CMD+=(\) -newermt "@$LAST_IBUP_TIME")
        MATCHED_FILES=$("${FIND_CMD[@]}")
    fi

    FILE_COUNT=$(echo "$MATCHED_FILES" | grep -c '^')

    if [ "$FILE_COUNT" -eq 0 ]; then
        echo "$taime No changes-Incremental backup was not created" >> "$rec"
    else
        # 3. Create next tar file

   ######################tar part of the code
        #in index we are getting number
        # ls gets all the files and 2/dev/null removes any error statement is empty
        # grep -Eo  E -> extended regex -o only show the matchig part
        # isbup -> starts with this ([0-9]+) one or more digits \.tar ends with tar
        # cut -d cuts f2 means get after dash    -> isbup-1.tar -> 1.tar
        # cut -d . -f splits on . and keeps the num like  1
        #then we sort  and use tail -n 1 to get the last digit
        # we add +1 to get number for next file
        INDEX=$(($(ls "$IBUP_DIR" 2>/dev/null | grep -Eo 'ibup-([0-9]+)\.tar' | cut -d'-' -f2 | cut -d'.' -f1 | sort -n | tail -n 1) + 1))
        INDEX=${INDEX:-1}
        saving_file="ibup-$INDEX.tar"
        echo "$MATCHED_FILES" | tar -cf "$IBUP_DIR/$saving_file" -T - 2>/dev/null
        echo "$taime $saving_file was created" >> "$rec"
    fi

    echo "[INFO] Sleeping 120s after incremental backup..."
    sleep 120
}
perform_differential_after_full() {
    #we are getting the date over here
    taime=$(date '+%a %d %b%Y %I:%M:%S %p %Z')

    # 1. Get taime of last full backup
    LAST_FBUP=$(ls -t "$FBUP_DIR"/fbup-*.tar 2>/dev/null | head -n 1)
    if [ -z "$LAST_FBUP" ]; then
        echo "$taime STEP 4 skipped: No full backup found" >> "$rec"
        sleep 120
        return
    fi
    LAST_FBUP_TIME=$(date -r "$LAST_FBUP" +%s)

    # 2. Find matching files modified after full backup
 ###################################find part of the file
    if [ "$#" -eq 0 ]; then
        MATCHED_FILES=$(find "$strt" -type f -newermt "@$LAST_FBUP_TIME")
    else
        FIND_CMD=(find "$strt" -type f \( )
        for EXT in "$@"; do
            FIND_CMD+=(-name "*$EXT" -o)
        done
        unset 'FIND_CMD[${#FIND_CMD[@]}-1]'
        FIND_CMD+=(\) -newermt "@$LAST_FBUP_TIME")
        MATCHED_FILES=$("${FIND_CMD[@]}")
    fi
# we are getting the number of the file
 ###################################################COUNT PART OF THE CODE
    FILE_COUNT=$(echo "$MATCHED_FILES" | grep -c '^')

    if [ "$FILE_COUNT" -eq 0 ]; then
        echo "$taime No changes-Differential backup was not created" >> "$rec"
    else
   ######################tar part of the code
        #in index we are getting number
        # ls gets all the files and 2/dev/null removes any error statement is empty
        # grep -Eo  E -> extended regex -o only show the matchig part
        # isbup -> starts with this ([0-9]+) one or more digits \.tar ends with tar
        # cut -d cuts f2 means get after dash    -> isbup-1.tar -> 1.tar
        # cut -d . -f splits on . and keeps the num like  1
        #then we sort  and use tail -n 1 to get the last digit
        # we add +1 to get number for next file
        INDEX=$(($(ls "$DBUP_DIR" 2>/dev/null | grep -Eo 'dbup-([0-9]+)\.tar' | cut -d'-' -f2 | cut -d'.' -f1 | sort -n | tail -n 1) + 1))
        INDEX=${INDEX:-1} #if we dont get any number then we do this
        saving_file="dbup-$INDEX.tar"
        echo "$MATCHED_FILES" | tar -cf "$DBUP_DIR/$saving_file" -T - 2>/dev/null
        echo "$taime $saving_file was created" >> "$rec"
    fi

    echo "[INFO] Sleeping 120s after differential backup..."
    sleep 120
}
perform_incremental_size_based() {

    taime=$(date '+%a %d %b%Y %I:%M:%S %p %Z')
    # 1. Get taime of last differential backup
     #we get the last diffre
     #we list all the files in dbup and pick the first one
     #2>/dev/null hides errors if no such file exists
    LAST_DBUP=$(ls -t "$DBUP_DIR"/dbup-*.tar 2>/dev/null | head -n 1)
    if [ -z "$LAST_DBUP" ]; then  #this checks if the variable is empty
        echo "$taime STEP 5 skipped: No differential backup found" >> "$rec"
        sleep 120 #we are sleeping for 2 min after no backup message
        return
    fi
    LAST_DBUP_TIME=$(date -r "$LAST_DBUP" +%s)

    # 2. Find matching files > 40KB modified after last differential
    if [ "$#" -eq 0 ]; then #this is if the arguments arent given then update all files more than 40k
        MATCHED_FILES=$(find "$strt" -type f -size +40k -newermt "@$LAST_DBUP_TIME")
    else
        #here we are building our command to find the specific type of file
        FIND_CMD=(find "$strt" -type f \( ) #we get all files in the root directory
        for EXT in "$@"; do  #we get all the arguments given by the user
            FIND_CMD+=(-name "*$EXT" -o) #we get name use -o which or
        done
        unset 'FIND_CMD[${#FIND_CMD[@]}-1]' #this removes the final -o in the argument
        FIND_CMD+=(\) -size +40k -newermt "@$LAST_DBUP_TIME") #this groups \   \
        MATCHED_FILES=$("${FIND_CMD[@]}") #execute the dynamically constructed find command
    fi
    #we count all the files and store it in file_count
    FILE_COUNT=$(echo "$MATCHED_FILES" | grep -c '^') #we count all the files
     #here we are checking if the count is empty
    if [ "$FILE_COUNT" -eq 0 ]; then #this logs the require message if no file counted
        echo "$taime No changes-Size-based incremental backup was not created" >> "$rec"
    else
        #lists existing isbup-*.tar files
        # Extracrs the number
        #Sorts them and picks the highest
         # adds 1
        #defaults t 1 if none exist
        INDEX=$(($(ls "$ISBUP_DIR" 2>/dev/null | grep -Eo 'isbup-([0-9]+)\.tar' | cut -d'-' -f2 | cut -d'.' -f1 | sort -n | tail -n 1) + 1))
        INDEX=${INDEX:-1} #no gunna be zero bro
        saving_file="isbup-$INDEX.tar"  #we get the new name for the files
        echo "$MATCHED_FILES" | tar -cf "$ISBUP_DIR/$saving_file" -T - 2>/dev/null
        echo "$taime $saving_file was created" >> "$rec"
    fi

    echo "[INFO] Sleeping 120s after size-based incremental backup..."
    sleep 120
}
#Collect file type arguments
FILE_TYPES=("$@")

#Continuous 5-step loop
while true; do
    #this is the first
    frstStepBckup "${FILE_TYPES[@]}"
    perform_incremental_after_full "${FILE_TYPES[@]}"
    perform_incremental_after_incremental "${FILE_TYPES[@]}"
    perform_differential_after_full "${FILE_TYPES[@]}"
    perform_incremental_size_based "${FILE_TYPES[@]}"
done


