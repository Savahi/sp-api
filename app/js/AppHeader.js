import React, { Component } from 'react';
import styles from './../css/appheader.css'; 

class AppHeader extends React.Component {
	render() {
		return (
			<div className={styles.container}>
				SP API Play Ground
			</div>
		);
	}
}

export default AppHeader;