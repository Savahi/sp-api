import React, { Component } from 'react';
import AppAPI from './AppAPI';
import AppRequest from './AppRequest';
import styles from './../css/appcontent.css'; 

class AppContent extends React.Component {
	render() {
		return (
			<div className={styles.container}>
				<AppRequest/>
				<h4>API List</h4>
				<AppAPI/>
			</div>
		);
	}
}

export default AppContent;