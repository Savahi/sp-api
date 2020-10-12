function getHost() {
    let host = window.location.hostname;
    if( window.location.port ) {
        if( window.location.port.length > 0 && window.location.port != 0 ) {
            host += ':' + window.location.port;
        }
    }
    host = window.location.protocol + '//' + host + '/';
    return host;
}

var Settings = {
    texts : {
        logInInfo: { 'en': 'Log-in Info', 'ru': 'Параметры входа' },
        user: { 'en': 'User', 'ru': 'Пользователь' },
        sessId: { 'en': 'Session Key', 'ru': 'Ключ сессии' },
        request: { 'en': 'Request', 'ru': 'Запрос' },
        makeRequest: { 'en': 'Make Request', 'ru': 'Послать запрос' },
        response: { 'en': 'Response', 'ru': 'Ответ' },
        apiList: { 'en': 'API List', 'ru': 'Список команд API' },
        parameters: { 'en': 'Parameters', 'ru': 'Параметры' },
        result: { 'en': 'Returns', 'ru': 'Возвращает' },
        parameters: { 'en': 'Parameters', 'ru': 'Параметры' },
        errorLoadingAPIList: { 'en': 'Error loading API List', 'ru':'Ошибка при загрузке списка API-команд'}
    },
	apiServerURL: getHost()
}

export default Settings;