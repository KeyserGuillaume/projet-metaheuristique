from __future__ import print_function
import pickle
import os.path
from googleapiclient.discovery import build
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request

# ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

# If modifying these scopes, delete the file token.pickle.
SCOPES = ['https://www.googleapis.com/auth/spreadsheets']

# The ID and range of a sample spreadsheet.
SAMPLE_SPREADSHEET_ID = '1nk6mTHk2dkoQg-BNINgm3_MCWTaMKCYRxnlgZQQHlvI'


def write_results_on_gsheet(letter, results):
    """
    Based on https://developers.google.com/sheets/api/quickstart/python?pli=1
    You can go on this page in order to generate a credentials.json which will
    enable you to use this function
    """
    creds = None
    # The file token.pickle stores the user's access and refresh tokens, and is
    # created automatically when the authorization flow completes for the first
    # time.
    if os.path.exists('token.pickle'):
        with open('token.pickle', 'rb') as token:
            creds = pickle.load(token)
    # If there are no (valid) credentials available, let the user log in.
    if not creds or not creds.valid:
        if creds and creds.expired and creds.refresh_token:
            creds.refresh(Request())
        else:
            flow = InstalledAppFlow.from_client_secrets_file(
                'credentials.json', SCOPES)
            creds = flow.run_local_server(port=0)
        # Save the credentials for the next run
        with open('token.pickle', 'wb') as token:
            pickle.dump(creds, token)

    service = build('sheets', 'v4', credentials=creds)

    # Call the Sheets API
    sheet = service.spreadsheets()
    # result = sheet.values().get(spreadsheetId=SAMPLE_SPREADSHEET_ID,
    #                             range='Data!A1:AAAAQ5').execute()

    # values = result.get('values', [])
    # idx = values.index(['v'])
    # last_letter = (int(len(results)/26) + 1)*"A" + ALPHABET[len(results) - int(len(results)/26)*26 + 1]
    myRange = 'Data!{}2:{}{}'.format(letter, letter, len(results) + 1)
    print(myRange)
    myBody = {'range': myRange,
              'values': [list(results)],
              'majorDimension': 'COLUMNS'}
    sheet.values().update(spreadsheetId=SAMPLE_SPREADSHEET_ID,
                          valueInputOption='RAW',
                          body=myBody,
                          range=myRange).execute()
